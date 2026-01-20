using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Numlock_Calc
{
    public partial class CalculatorForm : Form, IMessageFilter
    {
        private string currentCalculation = "";
        private bool isNewCalculation = true;

        private const int WM_HOTKEY = 0x0312;
        private const int VK_NUMLOCK = 0x90;
        private const uint MOD_NONE = 0x0000;
        private const uint KEYEVENTF_EXTENDEDKEY = 0x1;
        private const uint KEYEVENTF_KEYUP = 0x2;

        private bool originalNumLockState;

        [DllImport("user32.dll")]
        private static extern bool RegisterHotKey(IntPtr hWnd, int id, uint fsModifiers, uint vk);

        [DllImport("user32.dll")]
        private static extern bool UnregisterHotKey(IntPtr hWnd, int id);

        [DllImport("user32.dll")]
        private static extern short GetKeyState(int nVirtKey);

        [DllImport("user32.dll")]
        private static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);

        public CalculatorForm()
        {
            InitializeComponent();
            InitializeCalculator();
            InitializeTrayIcon();
            InitializeHotkeys();
            Application.AddMessageFilter(this);
        }

        private void InitializeCalculator()
        {
            displayTextBox.Text = "0";
            // Number buttons
            button0.Click += (s, e) => AppendNumber("0");
            button1.Click += (s, e) => AppendNumber("1");
            button2.Click += (s, e) => AppendNumber("2");
            button3.Click += (s, e) => AppendNumber("3");
            button4.Click += (s, e) => AppendNumber("4");
            button5.Click += (s, e) => AppendNumber("5");
            button6.Click += (s, e) => AppendNumber("6");
            button7.Click += (s, e) => AppendNumber("7");
            button8.Click += (s, e) => AppendNumber("8");
            button9.Click += (s, e) => AppendNumber("9");
            buttonDecimal.Click += (s, e) => AppendDecimal();

            // Operator buttons
            buttonAdd.Click += (s, e) => OperatorClick("+");
            buttonSubtract.Click += (s, e) => OperatorClick("-");
            buttonMultiply.Click += (s, e) => OperatorClick("*");
            buttonDivide.Click += (s, e) => OperatorClick("/");
            buttonPower.Click += (s, e) => OperatorClick("^");

            // Unary operator buttons
            buttonSqrt.Click += (s, e) => UnaryOperatorClick("√");
            buttonSquare.Click += (s, e) => UnaryOperatorClick("x²");
            buttonLog.Click += (s, e) => UnaryOperatorClick("log");

            // Other buttons
            buttonEquals.Click += (s, e) => EqualsClick();
            buttonC.Click += (s, e) => ClearAll();
            buttonCE.Click += (s, e) => ClearEntry();
            buttonBackspace.Click += (s, e) => Backspace();
            buttonNegate.Click += (s, e) => Negate();

            this.KeyDown += new KeyEventHandler(CalculatorForm_KeyDown);
        }

        private void InitializeTrayIcon()
        {
            string iconPath = System.IO.Path.Combine(Application.StartupPath, "calculator.ico");
            this.notifyIcon.Icon = new Icon(iconPath);
            this.notifyIcon.DoubleClick += (s, e) => RestoreFromTray();
            this.showMenuItem.Click += (s, e) => RestoreFromTray();
            this.exitMenuItem.Click += (s, e) => ExitApplication();
            this.Resize += (s, e) =>
            {
                if (this.WindowState == FormWindowState.Minimized)
                {
                    HideToTray();
                }
            };
        }

        private void InitializeHotkeys()
        {
            originalNumLockState = (GetKeyState(VK_NUMLOCK) & 1) != 0;
            RegisterHotKey(IntPtr.Zero, 0, MOD_NONE, VK_NUMLOCK);
            numLockTimer.Tick += (s, e) => EnforceNumLockOn();
            numLockTimer.Start();
        }

        private void AppendNumber(string number)
        {
            if (isNewCalculation)
            {
                currentCalculation = "";
                isNewCalculation = false;
            }
            currentCalculation += number;
            displayTextBox.Text = currentCalculation;
        }

        private void AppendDecimal()
        {
            if (isNewCalculation)
            {
                currentCalculation = "0";
                isNewCalculation = false;
            }
            if (!currentCalculation.Split(' ', '+', '-', '*', '/').Last().Contains("."))
            {
                currentCalculation += ".";
                displayTextBox.Text = currentCalculation;
            }
        }

        private void OperatorClick(string op)
        {
            if (isNewCalculation && currentCalculation.Length > 0)
            {
                isNewCalculation = false;
            }

            if (currentCalculation.Length > 0 && !IsLastCharOperator())
            {
                currentCalculation += $" {op} ";
                displayTextBox.Text = currentCalculation;
            }
        }

        private bool IsLastCharOperator()
        {
            if (string.IsNullOrWhiteSpace(currentCalculation)) return false;
            var lastChar = currentCalculation.Trim().Last();
            return lastChar == '+' || lastChar == '-' || lastChar == '*' || lastChar == '/' || lastChar == '^';
        }


        private void UnaryOperatorClick(string op)
        {
            try
            {
                double currentValue = double.Parse(currentCalculation.Split(' ', '+', '-', '*', '/').Last());
                double unaryResult = 0;
                string historyEntry = "";

                switch (op)
                {
                    case "√":
                        unaryResult = Calculator.SquareRoot(currentValue);
                        historyEntry = $"√({currentValue}) = {unaryResult}";
                        break;
                    case "x²":
                        unaryResult = Calculator.Square(currentValue);
                        historyEntry = $"({currentValue})² = {unaryResult}";
                        break;
                    case "log":
                        unaryResult = Calculator.Log(currentValue);
                        historyEntry = $"log({currentValue}) = {unaryResult}";
                        break;
                }

                // Replace the last number with the result
                var parts = currentCalculation.Split(' ').ToList();
                parts.RemoveAt(parts.Count - 1);
                currentCalculation = string.Join(" ", parts) + (parts.Any() ? " " : "") + unaryResult.ToString();


                displayTextBox.Text = unaryResult.ToString();
                historyListBox.Items.Add(historyEntry);
                isNewCalculation = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                ClearAll();
            }
        }

        private void EqualsClick()
        {
            if (isNewCalculation || string.IsNullOrEmpty(currentCalculation)) return;

            try
            {
                string expression = currentCalculation.Replace("^", "**"); // DataTable does not support ^
                var result = new DataTable().Compute(expression, null);
                string historyEntry = $"{currentCalculation} = {result}";
                historyListBox.Items.Add(historyEntry);

                currentCalculation = result?.ToString() ?? "";
                displayTextBox.Text = currentCalculation;
                isNewCalculation = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                ClearAll();
            }
        }

        private void ClearAll()
        {
            displayTextBox.Text = "0";
            currentCalculation = "";
            isNewCalculation = true;
        }

        private void ClearEntry()
        {
            if (currentCalculation.Length > 0)
            {
                var parts = currentCalculation.Trim().Split(' ').ToList();
                if (parts.Count > 0)
                {
                    parts.RemoveAt(parts.Count - 1);
                    currentCalculation = string.Join(" ", parts);
                    if (parts.Count > 0 && !IsLastCharOperator())
                    {
                        currentCalculation += " ";
                    }
                }
                displayTextBox.Text = string.IsNullOrEmpty(currentCalculation) ? "0" : currentCalculation;
            }
        }

        private void Backspace()
        {
            if (!string.IsNullOrEmpty(currentCalculation) && !isNewCalculation)
            {
                currentCalculation = currentCalculation.Trim();
                if (currentCalculation.Length > 0)
                {
                    currentCalculation = currentCalculation.Substring(0, currentCalculation.Length - 1).Trim();
                }

                displayTextBox.Text = string.IsNullOrEmpty(currentCalculation) ? "0" : currentCalculation;
            }
            else
            {
                ClearAll();
            }
        }

        private void Negate()
        {
            if (!string.IsNullOrEmpty(currentCalculation) && !isNewCalculation)
            {
                var parts = currentCalculation.Trim().Split(' ').ToList();
                if (parts.Count > 0)
                {
                    var lastPart = parts.Last();
                    if (double.TryParse(lastPart, out double number))
                    {
                        parts[parts.Count - 1] = (-number).ToString();
                        currentCalculation = string.Join(" ", parts);
                        displayTextBox.Text = currentCalculation;
                    }
                }
            }
        }

        private void CalculatorForm_KeyDown(object? sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.D0:
                case Keys.NumPad0:
                    AppendNumber("0");
                    break;
                case Keys.D1:
                case Keys.NumPad1:
                    AppendNumber("1");
                    break;
                case Keys.D2:
                case Keys.NumPad2:
                    AppendNumber("2");
                    break;
                case Keys.D3:
                case Keys.NumPad3:
                    AppendNumber("3");
                    break;
                case Keys.D4:
                case Keys.NumPad4:
                    AppendNumber("4");
                    break;
                case Keys.D5:
                case Keys.NumPad5:
                    AppendNumber("5");
                    break;
                case Keys.D6:
                case Keys.NumPad6:
                    AppendNumber("6");
                    break;
                case Keys.D7:
                case Keys.NumPad7:
                    AppendNumber("7");
                    break;
                case Keys.D8:
                case Keys.NumPad8:
                    AppendNumber("8");
                    break;
                case Keys.D9:
                case Keys.NumPad9:
                    AppendNumber("9");
                    break;
                case Keys.Decimal:
                    AppendDecimal();
                    break;
                case Keys.Add:
                    OperatorClick("+");
                    break;
                case Keys.Subtract:
                    OperatorClick("-");
                    break;
                case Keys.Multiply:
                    OperatorClick("*");
                    break;
                case Keys.Divide:
                    OperatorClick("/");
                    break;
                case Keys.Enter:
                    EqualsClick();
                    break;
                case Keys.Escape:
                    ClearAll();
                    break;
                case Keys.Back:
                    Backspace();
                    break;
            }
        }

        private void HideToTray()
        {
            this.WindowState = FormWindowState.Minimized;
            this.ShowInTaskbar = false;
        }

        private void RestoreFromTray()
        {
            this.Show();
            this.WindowState = FormWindowState.Normal;
            this.ShowInTaskbar = true;
            this.Activate();
        }

        private void ExitApplication()
        {
            numLockTimer.Stop();
            UnregisterHotKey(IntPtr.Zero, 0);
            if ((GetKeyState(VK_NUMLOCK) & 1) != 0 != originalNumLockState)
            {
                keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, UIntPtr.Zero);
                keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, UIntPtr.Zero);
            }
            Application.Exit();
        }
        private bool ensuringNumlockOn = false;
        private void EnforceNumLockOn()
        {
            if ((GetKeyState(VK_NUMLOCK) & 1) == 0)
            {
                ensuringNumlockOn = true;
                keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, UIntPtr.Zero);
                keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, UIntPtr.Zero);
            }
        }

        public bool PreFilterMessage(ref Message m)
        {
            if (m.Msg == WM_HOTKEY)
            {
                if (ensuringNumlockOn)
                {
                    ensuringNumlockOn = false;
                    return false;
                }
                if (this.WindowState == FormWindowState.Minimized)
                {
                    RestoreFromTray();
                }
                else
                {
                    HideToTray();
                }
                return true;
            }
            return false;
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            Application.RemoveMessageFilter(this);
            if (e.CloseReason == CloseReason.UserClosing)
            {
                e.Cancel = true;
                HideToTray();
            }
            else
            {
                base.OnFormClosing(e);
            }
        }
    }
}