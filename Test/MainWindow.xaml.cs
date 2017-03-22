using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Test
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

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
        public static extern bool GotoPage(int page);
        [DllImport("mupdftest", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Dispose(IntPtr pointer);

        public MainWindow()
        {
            InitializeComponent();

            // load a pdf from the web
            var webClient = new WebClient();
            byte[] imageBytes = webClient.DownloadData("http://cs.brown.edu/~peichmann/downloads/cted.pdf");

            // alternatively load pf from file system
            // var f = System.IO.Path.Combine(Environment.CurrentDirectory, "test.pdf");
            // var imageBytes = File.ReadAllBytes(f);

            var document = Open(imageBytes, imageBytes.Length);
            ActivateDocument(document);

            // Goto a page
            GotoPage(3);

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

                var bmp = LoadImage(mngdArray);
                var img = new Image();
                img.Source = bmp;
                xContainer.Children.Add(img);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
            finally
            {
                // Free up memory used by active pdf document
                Dispose(document);
            }
        }

        private static BitmapImage LoadImage(byte[] imageData)
        {
            if (imageData == null || imageData.Length == 0) return null;
            var image = new BitmapImage();
            using (var mem = new MemoryStream(imageData))
            {
                mem.Position = 0;
                image.BeginInit();
                image.CreateOptions = BitmapCreateOptions.PreservePixelFormat;
                image.CacheOption = BitmapCacheOption.OnLoad;
                image.UriSource = null;
                image.StreamSource = mem;
                image.EndInit();
            }
            image.Freeze();
            return image;
        }
    }
}
