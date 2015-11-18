using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace DeviceExplorer
{
    internal class CenterDialog : IDisposable
    {
        private readonly Form _owner;

        public CenterDialog(Form owner)
        {
            _owner = owner;
            owner.BeginInvoke(new MethodInvoker(FindDialog));
        }

        private void FindDialog()
        {
            EnumThreadWindows(GetCurrentThreadId(), CenterDialogBox, IntPtr.Zero);
        }

        private bool CenterDialogBox(IntPtr windowHandle, IntPtr lp)
        {
            var result = true;
            var sb = new StringBuilder(256);

            if (GetClassName(windowHandle, sb, sb.Capacity) != 0 && sb.ToString().Equals("#32770"))
            {
                var formRectangle = new System.Drawing.Rectangle(_owner.Location, _owner.Size);
                Rectangle dialogRectangle;
                if (GetWindowRect(windowHandle, out dialogRectangle) != 0)
                {
                    MoveWindow(windowHandle,
                        formRectangle.Left + (formRectangle.Width - dialogRectangle.Right + dialogRectangle.Left)/2,
                        formRectangle.Top + (formRectangle.Height - dialogRectangle.Bottom + dialogRectangle.Top)/2,
                        dialogRectangle.Right - dialogRectangle.Left,
                        dialogRectangle.Bottom - dialogRectangle.Top, true);
                    result = false;
                }
            }

            return result;
        }

        public void Dispose()
        {
        }

        private delegate bool EnumThreadWndProc(IntPtr windowHandle, IntPtr lp);
        [DllImport("user32.dll")]
        private static extern bool EnumThreadWindows(int tid, EnumThreadWndProc callback, IntPtr lp);
        [DllImport("kernel32.dll")]
        private static extern int GetCurrentThreadId();
        [DllImport("user32.dll")]
        private static extern int GetClassName(IntPtr hWnd, StringBuilder buffer, int buflen);
        [DllImport("user32.dll")]
        private static extern int GetWindowRect(IntPtr hWnd, out Rectangle rc);
        [DllImport("user32.dll")]
        private static extern bool MoveWindow(IntPtr hWnd, int x, int y, int w, int h, bool repaint);
        private struct Rectangle {
            public int Left { get; }
            public int Top { get; }
            public int Right { get; }
            public int Bottom { get; }
        }
    }
}