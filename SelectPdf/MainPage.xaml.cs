using Microsoft.Graphics.Canvas;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Input;
using Windows.UI.Input.Inking;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;
using Windows.UI.Core;
using Windows.UI;
using Windows.UI.Input.Inking.Core;

namespace SelectPdf
{
    public struct MuPoint
    {
        public int x;
        public int y;
    };

    public struct MuRect
    {
        public int x0, y0;
        public int x1, y1;
    };

    enum ContentType {
        TEXT_CONTENT = 0,
        IMAGE_CONTENT = 1
    };

    [StructLayout(LayoutKind.Explicit)]
    public struct MuSelection
    {
        [FieldOffset(0)]
        public int type;
        [FieldOffset(4)]
        public int numBytes;
        [FieldOffset(8)]
        public IntPtr content;
    };

public sealed partial class MainPage : Page
    {
        private const int MAX_NUM = 1024;
        private const int MAX_SEL_NUM = 10;

        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Open(byte[] data, int length);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ActivateDocument(IntPtr document);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Free(IntPtr pointer);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int RenderPage(int width, int height);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetTextBytes(byte[] sb);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetBuffer();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int RenderToPng(int width, int height, IntPtr data);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetPageWidth();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetPageHeight();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetNumComponents();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetNumPages();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetCurrentPageNum();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool GotoPage(int page);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool GoToNextPage();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool GoToPrevPage();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Dispose(IntPtr pointer);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int AddSelection(MuPoint[] pointList, int size);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetHighlights(int id, 
            [MarshalAs(UnmanagedType.LPArray, SizeConst = MAX_NUM)] ref MuRect[] rectList,
            int size);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetNumSelections();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetSelectionContent(int id);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetSelectionContents(int id,
            [MarshalAs(UnmanagedType.LPArray, SizeConst = MAX_SEL_NUM)] ref MuSelection[] contentList,
            int size);

        private IntPtr currentDocument = IntPtr.Zero;
        private int currentPageNum = -1;
        private RichEditBox currentRtf = new RichEditBox();

        private List<MuRect> highlightList = new List<MuRect>();
        private CanvasBitmap pdfIm;
        private int width;

        // map of page number to list of selections on that page
        private Dictionary<int, List<Selection>> selectionMap = new Dictionary<int, List<Selection>>();

        // this is needed for the dll
        private static byte[] file;

        public MainPage()
        {
            this.InitializeComponent();

            inkCanvas.InkPresenter.InputDeviceTypes = Windows.UI.Core.CoreInputDeviceTypes.Mouse | Windows.UI.Core.CoreInputDeviceTypes.Pen;
            InkDrawingAttributes inkAttributes = inkCanvas.InkPresenter.CopyDefaultDrawingAttributes();
            inkAttributes.Size = new Size(10, 5);
            inkAttributes.Color = Colors.CadetBlue;
            inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(inkAttributes);

            inkCanvas.InkPresenter.StrokesCollected += InkPresenter_StrokesCollected;
            inkCanvas.PointerWheelChanged += new PointerEventHandler(InkCanvas_PointerWheel);
        }

        private void InkCanvas_PointerWheel(object sender, PointerRoutedEventArgs e)
        {
            var d = e.GetCurrentPoint(sender as UIElement).Properties.MouseWheelDelta;
            bool result;
            int updatedPage;
            
            // determine if scrolling up or down
            if (d < 0)
            {
                result = GoToNextPage();
                updatedPage = currentPageNum + 1;
            }
            else
            {
                result = GoToPrevPage();
                updatedPage = currentPageNum - 1;
            }

            if (result)
            {
                // update page num
                currentPageNum = updatedPage;
                // update highlight list to reflect updated page
                highlightList.Clear();
                if (selectionMap.ContainsKey(currentPageNum))
                {
                    foreach(Selection s in selectionMap[currentPageNum])
                    {
                        highlightList.AddRange(s.GetHighlightRects());
                    }
                }
                // render updated page
                Render();
            }
            e.Handled = true;
        }

        private void InkPresenter_StrokesCollected(InkPresenter sender, InkStrokesCollectedEventArgs args)
        {
            List<MuPoint> pointList = new List<MuPoint>();

            // get user stroke points from ink and convert to MuPoint list
            foreach (InkStroke stroke in sender.StrokeContainer.GetStrokes())
            {
                foreach (InkPoint p in stroke.GetInkPoints())
                {
                    pointList.Add(GetPdfPoint(p.Position.X, p.Position.Y));
                }
            }
            inkCanvas.InkPresenter.StrokeContainer.Clear(); // clear the ink from the canvas

            // add selection to DLL using the list of MuPoints
            int selected = AddSelection(pointList.ToArray(), pointList.Count);

            if (selected > -1)
            {
                // keep the new selection in the frontend
                Selection currSel = new Selection(selected, currentPageNum);

                MuRect[] rectList = new MuRect[MAX_NUM];
                int num = GetHighlights(selected, ref rectList, MAX_NUM);
                Debug.WriteLine("output" + num);

                for (int k = 0; k < num; k++)
                {
                    // add highlight rects to both the selection object and current highlight list
                    currSel.AddHighlightRect(rectList[k]);
                    highlightList.Add(rectList[k]);
                }

                // add the new selection to selection map under current page num
                if (selectionMap.ContainsKey(currentPageNum))
                {
                    selectionMap[currentPageNum].Add(currSel);
                } else
                {
                    selectionMap.Add(currentPageNum, new List<Selection> { currSel });
                }
            }

            selectCanvas.Invalidate();
        }

        private MuPoint GetPdfPoint(double x, double y)
        {
            double marg = (selectCanvas.ActualWidth - width) / 2;

            MuPoint point = new MuPoint();
            point.y = Convert.ToInt32(y);
            point.x = Convert.ToInt32(x - marg);
            return point;
        }

        private static async Task LoadFile()
        {
            var openPicker = new FileOpenPicker();
            openPicker.SuggestedStartLocation = PickerLocationId.DocumentsLibrary;
            openPicker.FileTypeFilter.Add(".pdf");
            StorageFile selectedFile = await openPicker.PickSingleFileAsync();

            using (Stream stream = await selectedFile.OpenStreamForReadAsync())
            {
                using (var memoryStream = new MemoryStream())
                {
                    stream.CopyTo(memoryStream);
                    file = memoryStream.ToArray();
                }
            }
        }

        private async Task<BitmapImage> ByteArrayToBitmapImage(byte[] byteArray, bool addToRtf)
        {
            if (byteArray != null)
            {
                using (var stream = new InMemoryRandomAccessStream())
                {
                    await stream.WriteAsync(byteArray.AsBuffer());
                    var image = new BitmapImage();
                    stream.Seek(0);
                    image.SetSource(stream);
                    if (addToRtf)
                    {
                        currentRtf.Document.Selection.InsertImage(image.PixelWidth, image.PixelHeight, 0, Windows.UI.Text.VerticalCharacterAlignment.Baseline, "Image", stream);
                    }
                    return image;
                }
            }
            return null;
        }

        private async Task<RandomAccessStreamReference> ByteArrayToStreamRef(byte[] byteArray)
        {
            if (byteArray != null)
            {
                using (var stream = new InMemoryRandomAccessStream())
                {
                    await stream.WriteAsync(byteArray.AsBuffer());

                    return RandomAccessStreamReference.CreateFromStream(stream);
                }
            }
            return null;
        }

        private async void Render()
        {
            // Get aspect ratio of the page
            var aspectRatio = GetPageWidth() / (double)GetPageHeight();

            // Render the Page
            var size = (int)selectCanvas.ActualHeight; // size in pixels
            width = (int)(size * aspectRatio);
            var numBytes = RenderPage(width, size);

            // Get a reference to the buffer that contains the rendererd page
            var buffer = GetBuffer();

            // Copy the buffer from unmanaged to managed memory (mngdArray contains the bytes of the pdf page rendered as PNG)
            byte[] mngdArray = new byte[numBytes];

            try
            {
                Marshal.Copy(buffer, mngdArray, 0, numBytes);

                using (var stream = new InMemoryRandomAccessStream())
                {
                    await stream.WriteAsync(mngdArray.AsBuffer());
                    pdfIm = await CanvasBitmap.LoadAsync(selectCanvas, stream);
                    selectCanvas.Invalidate();
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
            }
        }

        private async void Load_Button_Click(object sender, RoutedEventArgs e)
        {
            //var dataPackageView = Clipboard.GetContent();
            //if (dataPackageView.Contains(StandardDataFormats.Text))
            //{
            //    try
            //    {
            //        var text = await dataPackageView.GetTextAsync();
            //        Debug.WriteLine(text);
            //    }
            //    catch (Exception ex)
            //    {
            //    }
            //}
            //if (dataPackageView.Contains(StandardDataFormats.Bitmap))
            //{
            //    Debug.WriteLine("bitmap");
            //}
            //if (dataPackageView.Contains(StandardDataFormats.Html))
            //{
            //    Debug.WriteLine("html");
            //}
            //if (dataPackageView.Contains(StandardDataFormats.Rtf))
            //{
            //    var r = await dataPackageView.GetRtfAsync();
            //    Debug.WriteLine("rtf");
            //    Debug.WriteLine(r);
            //}
            //if (dataPackageView.Contains(StandardDataFormats.StorageItems))
            //{
            //    Debug.WriteLine("storage items");
            //}
            //if (dataPackageView.Contains(StandardDataFormats.Uri))
            //{
            //    Debug.WriteLine("uri");
            //}

            //var dataPackage = new DataPackage { RequestedOperation = DataPackageOperation.Copy };
            ////dataPackage.SetData("AnsiText", "cry");

            //var imagePicker = new FileOpenPicker

            //{

            //    ViewMode = PickerViewMode.Thumbnail,

            //    SuggestedStartLocation = PickerLocationId.PicturesLibrary,

            //    FileTypeFilter = { ".jpg", ".png", ".bmp", ".gif", ".tif" }

            //};

            //string cu;
            //sad.Document.Selection.GetText(Windows.UI.Text.TextGetOptions.None, out cu);
            //sad.Document.Selection.SetText(Windows.UI.Text.TextSetOptions.ApplyRtfDocumentDefaults, cu + "omg so sad");

            //var imageFile = await imagePicker.PickSingleFileAsync();
            //string content;

            //using (IRandomAccessStream stream = await imageFile.OpenAsync(FileAccessMode.Read))
            //{
            //    BitmapImage image = new BitmapImage();
            //    await image.SetSourceAsync(stream);
            //    sad.Document.Selection.InsertImage(image.PixelWidth, image.PixelHeight, 0, Windows.UI.Text.VerticalCharacterAlignment.Baseline, "Image", stream);
            //}

            //string cu2;
            //sad.Document.Selection.GetText(Windows.UI.Text.TextGetOptions.AdjustCrlf, out cu2);
            //Debug.WriteLine(cu2);
            //sad.Document.Selection.SetText(Windows.UI.Text.TextSetOptions.ApplyRtfDocumentDefaults, cu2 + "more text");


            // Ask for RTF here, if desired.
            //string temp;
            // Do not ask for RTF here, we just want the raw text
            //sad.Document.GetText(Windows.UI.Text.TextGetOptions.None, out temp);
            //var range = sad.Document.GetRange(0, temp.Length - 1);

            // Ask for RTF here, if desired.
            //range.GetText(Windows.UI.Text.TextGetOptions.FormatRtf, out content);
            //sad.Document.Selection.GetText(Windows.UI.Text.TextGetOptions.FormatRtf, out content);

            //dataPackage.SetRtf(content);
            //Debug.WriteLine("here");
            //Debug.WriteLine(content);
            //Clipboard.SetContent(dataPackage);

            //dataPackage.SetBitmap(RandomAccessStreamReference.CreateFromFile(imageFile));

            //Clipboard.SetContent(dataPackage);

            
            await LoadFile();

            currentDocument = Open(file, file.Length);
            ActivateDocument(currentDocument);
            currentPageNum = 0;

            Render();
        }

        private async void Selection_Button_Click(object sender, RoutedEventArgs e)
        {
            if (selectionContainer.Visibility == Visibility.Collapsed)
            {
                selectionContainer.Visibility = Visibility.Visible;

                int numSelections = GetNumSelections();

                for (int i = 0; i < numSelections; i++)
                {
                    selectionContainer.RowDefinitions.Add(new RowDefinition());
                    Grid selectionGrid = new Grid();
                    Grid.SetRow(selectionGrid, i);

                    MuSelection[] contentList = new MuSelection[MAX_SEL_NUM];
                    int numContents = GetSelectionContents(i, ref contentList, MAX_SEL_NUM);

                    string selectionText = "";
                    RandomAccessStreamReference selectionImage = null;

                    for (int j = 0; j < numContents; j++)
                    {
                        selectionGrid.RowDefinitions.Add(new RowDefinition());

                        if (contentList[j].type == (int)ContentType.IMAGE_CONTENT)
                        {
                            IntPtr bytes = contentList[j].content;
                            byte[] mngdArray = new byte[contentList[j].numBytes];
                            try
                            {
                                Marshal.Copy(bytes, mngdArray, 0, contentList[j].numBytes);

                                var bmp = await ByteArrayToBitmapImage(mngdArray, true);
                                selectionImage = await ByteArrayToStreamRef(mngdArray);
                                var img = new Image();
                                img.Source = bmp;
                                Grid.SetRow(img, j);
                                selectionGrid.Children.Add(img);
                            }
                            catch (Exception ex)
                            {
                                Debug.WriteLine(ex);
                            }
                        } else
                        {
                            IntPtr bytes = contentList[j].content;
                            string selText = Marshal.PtrToStringAnsi(bytes);
                            selectionText += selText;
                            TextBlock txt = new TextBlock();
                            txt.Text = selText;
                            Grid.SetRow(txt, j);
                            selectionGrid.Children.Add(txt);
                        }
                    }

                    currentRtf.Document.Selection.SetText(Windows.UI.Text.TextSetOptions.ApplyRtfDocumentDefaults, selectionText);

                    string temp, content;
                    currentRtf.Document.GetText(Windows.UI.Text.TextGetOptions.None, out temp);
                    var range = currentRtf.Document.GetRange(0, temp.Length - 1);
                    range.GetText(Windows.UI.Text.TextGetOptions.FormatRtf, out content);

                    var dataPackage = new DataPackage { RequestedOperation = DataPackageOperation.Copy };
                    dataPackage.SetRtf(content);
                    dataPackage.SetText(selectionText);
                    if (selectionImage != null)
                    {
                        dataPackage.SetBitmap(selectionImage);
                    }
                    Clipboard.SetContent(dataPackage);

                    selectionContainer.Children.Add(selectionGrid);
                }

            } else
            {
                selectionContainer.Visibility = Visibility.Collapsed;
            }

        }

        private void selectCanvas_Draw(Microsoft.Graphics.Canvas.UI.Xaml.CanvasControl sender, Microsoft.Graphics.Canvas.UI.Xaml.CanvasDrawEventArgs args)
        {
            if (pdfIm != null)
            {
                int marg = (int)((selectCanvas.ActualWidth - width) / 2);
                args.DrawingSession.DrawImage(pdfIm, marg, 0);

                if (highlightList.Count > 0)
                {
                    foreach (MuRect rect in highlightList)
                    {
                        args.DrawingSession.DrawRectangle(rect.x0 + marg, rect.y0, rect.x1 - rect.x0, rect.y1 - rect.y0, Colors.Red);
                    }
                }
            }
        }

        void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            selectCanvas.RemoveFromVisualTree();
            selectCanvas = null;
        }
        
    }
}
