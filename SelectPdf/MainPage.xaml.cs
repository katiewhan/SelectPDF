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
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Input;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

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

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private const int MAX_NUM = 1024;

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
        public static extern bool GotoPage(int page);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GoToNextPage();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GoToPrevPage();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Dispose(IntPtr pointer);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool AddSelection(MuPoint[] pointList, int size);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetHighlights([MarshalAs(UnmanagedType.LPArray, SizeConst = MAX_NUM)] ref MuRect[] rectList, int size);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetNumSelections();
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetSelectionContent(int index);

        private IntPtr currentDocument = IntPtr.Zero;
        private List<MuPoint> pointList = new List<MuPoint>();
        private bool selecting = false;
        private int currentPageNum = -1;

        private static byte[] file;

        public MainPage()
        {
            this.InitializeComponent();

            pdfContainer.PointerPressed += new PointerEventHandler(pdfContainer_PointerPressed);
            pdfContainer.PointerReleased += new PointerEventHandler(pdfContainer_PointerReleased);
            pdfContainer.PointerMoved += new PointerEventHandler(pdfContainer_PointerMoved);

            pdfContainer.PointerWheelChanged += new PointerEventHandler(pdfContainer_PointerWheel);
        }

        private MuPoint GetPdfPoint(double x, double y)
        {
            double size = 2000; // size in pixels

            Image pdf = new Image();
            foreach(Image o in pdfContainer.Children.OfType<Image>())
            {
                pdf = o;
            }
            double offsetY = (pdfContainer.ActualHeight - pdf.ActualHeight) / 2.0;
            double offsetX = (pdfContainer.ActualWidth - pdf.ActualWidth) / 2.0;
            double aspectRatio = pdf.ActualWidth / pdf.ActualHeight;
            double heightRatio = size / pdf.ActualHeight;
            double widthRatio = (size * aspectRatio) / pdf.ActualWidth;

            MuPoint point = new MuPoint();
            point.y = Convert.ToInt32((y - offsetY) * heightRatio);
            point.x = Convert.ToInt32((x - offsetX) * widthRatio);
            return point;
        }

        private void DrawRect(MuRect rect)
        {
            double size = 2000; // size in pixels

            Image pdf = new Image();
            foreach (Image o in pdfContainer.Children.OfType<Image>())
            {
                pdf = o;
            } // get currentPage
            double offsetY = (pdfContainer.ActualHeight - pdf.ActualHeight) / 2.0;
            double offsetX = (pdfContainer.ActualWidth - pdf.ActualWidth) / 2.0;
            double aspectRatio = pdf.ActualWidth / pdf.ActualHeight;
            double heightRatio = size / pdf.ActualHeight;
            double widthRatio = (size * aspectRatio) / pdf.ActualWidth;

            Rectangle r = new Rectangle();
            r.Fill = new SolidColorBrush(Windows.UI.Colors.LightYellow);
            r.Fill.Opacity = 0.7;
            r.Width = (rect.x1 - rect.x0) / widthRatio;
            r.Height = (rect.y1 - rect.y0) / heightRatio;
            r.Margin = new Thickness((rect.x0 / widthRatio) + offsetX, (rect.y0 / heightRatio) + offsetY, 0, 0);
            pdfCanvas.Children.Add(r);
            Canvas.SetZIndex(pdfCanvas, 10);

        }

        private void pdfContainer_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            if (pdfContainer.Children.Count > 1 )
            {
                var p = e.GetCurrentPoint(sender as UIElement).Position;
                pointList.Add(GetPdfPoint(p.X, p.Y));
                selecting = true;
            }

            e.Handled = true;
        }

        private void pdfContainer_PointerReleased(object sender, PointerRoutedEventArgs e)
        {   
            bool selected = AddSelection(pointList.ToArray(), pointList.Count);
            Debug.WriteLine(selected);
            
            if (selected)
            {
                MuRect[] rectList = new MuRect[MAX_NUM];
                int num = GetHighlights(ref rectList, MAX_NUM);
                //var r = Marshal.PtrToStructure<FzRect>(ptr);
                //string c = Marshal.PtrToStringAnsi(output);
                //Marshal.FreeHGlobal(output);
                Debug.WriteLine("output");
                Debug.WriteLine(num);

                for (int k = 0; k < num; k++)
                {
                    //MuRect t = rectList[k];
                    DrawRect(rectList[k]);
                }
            }

            pointList.Clear();
            selecting = false;
            e.Handled = true;
        }

        private void pdfContainer_PointerMoved(object sender, PointerRoutedEventArgs e)
        {
            if (selecting && pdfContainer.Children.Count > 1)
            {
                var p = e.GetCurrentPoint(sender as UIElement).Position;
                pointList.Add(GetPdfPoint(p.X, p.Y));
            }
            e.Handled = true;
        }

        private void pdfContainer_PointerWheel(object sender, PointerRoutedEventArgs e)
        {
            var d = e.GetCurrentPoint(sender as UIElement).Properties.MouseWheelDelta;
            bool result;
            int updatedPage;
            if (d < 0)
            {
                result = GoToNextPage();
                updatedPage = currentPageNum + 1;
            } else
            {
                result = GoToPrevPage();
                updatedPage = currentPageNum - 1;
            }

            if (result)
            {
                //if (currentPageNum <= pdfContainer.Children.Count)
                //{
                //    // show updatedPage
                //    // hide currentPage
                //} else
                //{
                Render();
                //}
                currentPageNum = updatedPage;

            }
            e.Handled = true;
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

        private static async Task<BitmapImage> ByteArrayToBitmapImage(byte[] byteArray)
        {
            if (byteArray != null)
            {
                using (var stream = new InMemoryRandomAccessStream())
                {
                    await stream.WriteAsync(byteArray.AsBuffer());
                    var image = new BitmapImage();
                    stream.Seek(0);
                    image.SetSource(stream);
                    return image;
                }
            }
            return null;
        }

        private async void Render()
        {
            // Get aspect ratio of the page
            var aspectRatio = GetPageWidth() / (double)GetPageHeight();

            // Render the Page
            var size = 2000; // size in pixels
            var numBytes = RenderPage((int)(size * aspectRatio), size);

            // Get a reference to the buffer that contains the rendererd page
            var buffer = GetBuffer();

            // Copy the buffer from unmanaged to managed memory (mngdArray contains the bytes of the pdf page rendered as PNG)
            byte[] mngdArray = new byte[numBytes];

            try
            {
                Marshal.Copy(buffer, mngdArray, 0, numBytes);

                var bmp = await ByteArrayToBitmapImage(mngdArray);
                var img = new Image();
                img.Source = bmp;
                img.Name = "Page" + currentPageNum;
                pdfContainer.Children.Add(img);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
            }
            //finally
            //{
            //    // Free up memory used by active pdf document
            //    Dispose(currentDocument);
            //}
        }

        private async void Load_Button_Click(object sender, RoutedEventArgs e)
        {

            await LoadFile();

            currentDocument = Open(file, file.Length);
            ActivateDocument(currentDocument);
            currentPageNum = 0;

            Render();
        }

        private void Selection_Button_Click(object sender, RoutedEventArgs e)
        {
            IntPtr text = GetSelectionContent(0);
            string c = Marshal.PtrToStringAnsi(text);
            Debug.Write(c);

        }
    }
}
