using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace DeviceExplorer
{
    class CenterDialog : IDisposable
    {
        private int _tries;
        private readonly Form _owner;

        public CenterDialog(Form owner)
        {
            _owner = owner;
            owner.BeginInvoke(new MethodInvoker(FindDialog));
        }

        private void FindDialog()
        {
            if (_tries < 0) return;
            var callback = new EnumThreadWndProc(CheckWindow);
            if (EnumThreadWindows(GetCurrentThreadId(), callback, IntPtr.Zero))
            {
                if (++_tries < 10) _owner.BeginInvoke(new MethodInvoker(FindDialog));
            }
        }
        private bool CheckWindow(IntPtr hWnd, IntPtr lp)
        {
            var sb = new StringBuilder(260);
            GetClassName(hWnd, sb, sb.Capacity);
            if (sb.ToString() != "#32770")
            {
                return true;
            }
            var formRectangle = new Rectangle(_owner.Location, _owner.Size);
            Rect dialogRectangle;
            GetWindowRect(hWnd, out dialogRectangle);
            MoveWindow(hWnd,
                formRectangle.Left + (formRectangle.Width - dialogRectangle.Right + dialogRectangle.Left) / 2,
                formRectangle.Top + (formRectangle.Height - dialogRectangle.Bottom + dialogRectangle.Top) / 2,
                dialogRectangle.Right - dialogRectangle.Left,
                dialogRectangle.Bottom - dialogRectangle.Top, true);
            return false;
        }
        public void Dispose()
        {
            _tries = -1;
        }

        private delegate bool EnumThreadWndProc(IntPtr hWnd, IntPtr lp);
        [DllImport("user32.dll")]
        private static extern bool EnumThreadWindows(int tid, EnumThreadWndProc callback, IntPtr lp);
        [DllImport("kernel32.dll")]
        private static extern int GetCurrentThreadId();
        [DllImport("user32.dll")]
        private static extern int GetClassName(IntPtr hWnd, StringBuilder buffer, int buflen);
        [DllImport("user32.dll")]
        private static extern bool GetWindowRect(IntPtr hWnd, out Rect rc);
        [DllImport("user32.dll")]
        private static extern bool MoveWindow(IntPtr hWnd, int x, int y, int w, int h, bool repaint);
        private struct Rect { public int Left; public int Top; public int Right; public int Bottom; }
    }
}