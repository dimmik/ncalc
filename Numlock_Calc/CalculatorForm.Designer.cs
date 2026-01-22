namespace Numlock_Calc
{
    partial class CalculatorForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            components = new System.ComponentModel.Container();
            displayTextBox = new TextBox();
            historyListBox = new ListBox();
            contextMenuStrip = new ContextMenuStrip(components);
            showMenuItem = new ToolStripMenuItem();
            exitMenuItem = new ToolStripMenuItem();
            clearHistoryMenuItem = new ToolStripMenuItem();
            tableLayoutPanel = new TableLayoutPanel();
            buttonEquals = new Button();
            buttonDecimal = new Button();
            button0 = new Button();
            buttonNegate = new Button();
            buttonAdd = new Button();
            button3 = new Button();
            button2 = new Button();
            button1 = new Button();
            buttonSubtract = new Button();
            button6 = new Button();
            button5 = new Button();
            button4 = new Button();
            buttonMultiply = new Button();
            button9 = new Button();
            button8 = new Button();
            button7 = new Button();
            buttonDivide = new Button();
            buttonBackspace = new Button();
            buttonC = new Button();
            buttonOpenParenthesis = new Button();
            buttonCloseParenthesis = new Button();
            buttonSqrt = new Button();
            buttonSquare = new Button();
            buttonPower = new Button();
            buttonLog = new Button();
            notifyIcon = new NotifyIcon(components);
            numLockTimer = new System.Windows.Forms.Timer(components);
            contextMenuStrip.SuspendLayout();
            tableLayoutPanel.SuspendLayout();
            SuspendLayout();
            // 
            // displayTextBox
            // 
            displayTextBox.Dock = DockStyle.Top;
            displayTextBox.Font = new Font("Segoe UI", 16F);
            displayTextBox.Location = new Point(0, 0);
            displayTextBox.Name = "displayTextBox";
            displayTextBox.ReadOnly = true;
            displayTextBox.Size = new Size(384, 36);
            displayTextBox.TabIndex = 0;
            displayTextBox.TextAlign = HorizontalAlignment.Right;
            // 
            // historyListBox
            // 
            historyListBox.ContextMenuStrip = contextMenuStrip;
            historyListBox.Dock = DockStyle.Top;
            historyListBox.DrawMode = DrawMode.OwnerDrawFixed;
            historyListBox.FormattingEnabled = true;
            historyListBox.ItemHeight = 30;
            historyListBox.Location = new Point(0, 36);
            historyListBox.Name = "historyListBox";
            historyListBox.ScrollAlwaysVisible = true;
            historyListBox.Size = new Size(384, 154);
            historyListBox.TabIndex = 1;
            historyListBox.DrawItem += historyListBox_DrawItem;
            historyListBox.DoubleClick += historyListBox_DoubleClick;
            // 
            // contextMenuStrip
            // 
            contextMenuStrip.Items.AddRange(new ToolStripItem[] { showMenuItem, exitMenuItem, clearHistoryMenuItem });
            contextMenuStrip.Name = "contextMenuStrip";
            contextMenuStrip.Size = new Size(161, 70);
            // 
            // showMenuItem
            // 
            showMenuItem.Name = "showMenuItem";
            showMenuItem.Size = new Size(160, 22);
            showMenuItem.Text = "Show Calculator";
            // 
            // exitMenuItem
            // 
            exitMenuItem.Name = "exitMenuItem";
            exitMenuItem.Size = new Size(160, 22);
            exitMenuItem.Text = "Exit";
            // 
            // clearHistoryMenuItem
            // 
            clearHistoryMenuItem.Name = "clearHistoryMenuItem";
            clearHistoryMenuItem.Size = new Size(160, 22);
            clearHistoryMenuItem.Text = "Clear History";
            // 
            // tableLayoutPanel
            // 
            tableLayoutPanel.ColumnCount = 4;
            tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25F));
            tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25F));
            tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25F));
            tableLayoutPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25F));
            tableLayoutPanel.Controls.Add(buttonEquals, 3, 5);
            tableLayoutPanel.Controls.Add(buttonDecimal, 2, 5);
            tableLayoutPanel.Controls.Add(button0, 1, 5);
            tableLayoutPanel.Controls.Add(buttonNegate, 0, 5);
            tableLayoutPanel.Controls.Add(buttonAdd, 3, 4);
            tableLayoutPanel.Controls.Add(button3, 2, 4);
            tableLayoutPanel.Controls.Add(button2, 1, 4);
            tableLayoutPanel.Controls.Add(button1, 0, 4);
            tableLayoutPanel.Controls.Add(buttonSubtract, 3, 3);
            tableLayoutPanel.Controls.Add(button6, 2, 3);
            tableLayoutPanel.Controls.Add(button5, 1, 3);
            tableLayoutPanel.Controls.Add(button4, 0, 3);
            tableLayoutPanel.Controls.Add(buttonMultiply, 3, 2);
            tableLayoutPanel.Controls.Add(button9, 2, 2);
            tableLayoutPanel.Controls.Add(button8, 1, 2);
            tableLayoutPanel.Controls.Add(button7, 0, 2);
            tableLayoutPanel.Controls.Add(buttonDivide, 3, 1);
            tableLayoutPanel.Controls.Add(buttonBackspace, 2, 1);
            tableLayoutPanel.Controls.Add(buttonC, 1, 1);
            tableLayoutPanel.Controls.Add(buttonOpenParenthesis, 0, 1);
            tableLayoutPanel.Controls.Add(buttonCloseParenthesis, 1, 1);
            tableLayoutPanel.Controls.Add(buttonSqrt, 0, 0);
            tableLayoutPanel.Controls.Add(buttonSquare, 1, 0);
            tableLayoutPanel.Controls.Add(buttonPower, 2, 0);
            tableLayoutPanel.Controls.Add(buttonLog, 3, 0);
            tableLayoutPanel.Dock = DockStyle.Fill;
            tableLayoutPanel.Location = new Point(0, 190);
            tableLayoutPanel.Name = "tableLayoutPanel";
            tableLayoutPanel.RowCount = 6;
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 9.473684F));
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 12.1052628F));
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 17.4825172F));
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 17.8321686F));
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 18.181818F));
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 25.1748257F));
            tableLayoutPanel.RowStyles.Add(new RowStyle(SizeType.Absolute, 116F));
            tableLayoutPanel.Size = new Size(384, 404);
            tableLayoutPanel.TabIndex = 2;
            // 
            // buttonEquals
            // 
            buttonEquals.Dock = DockStyle.Fill;
            buttonEquals.Location = new Point(3, 289);
            buttonEquals.Name = "buttonEquals";
            buttonEquals.Size = new Size(90, 112);
            buttonEquals.TabIndex = 23;
            buttonEquals.Text = "=";
            buttonEquals.UseVisualStyleBackColor = true;
            // 
            // buttonDecimal
            // 
            buttonDecimal.Dock = DockStyle.Fill;
            buttonDecimal.Location = new Point(291, 217);
            buttonDecimal.Name = "buttonDecimal";
            buttonDecimal.Size = new Size(90, 66);
            buttonDecimal.TabIndex = 22;
            buttonDecimal.Text = ".";
            buttonDecimal.UseVisualStyleBackColor = true;
            // 
            // button0
            // 
            button0.Dock = DockStyle.Fill;
            button0.Location = new Point(195, 217);
            button0.Name = "button0";
            button0.Size = new Size(90, 66);
            button0.TabIndex = 21;
            button0.Text = "0";
            button0.UseVisualStyleBackColor = true;
            // 
            // buttonNegate
            // 
            buttonNegate.Dock = DockStyle.Fill;
            buttonNegate.Location = new Point(99, 217);
            buttonNegate.Name = "buttonNegate";
            buttonNegate.Size = new Size(90, 66);
            buttonNegate.TabIndex = 20;
            buttonNegate.Text = "+/-";
            buttonNegate.UseVisualStyleBackColor = true;
            // 
            // buttonAdd
            // 
            buttonAdd.Dock = DockStyle.Fill;
            buttonAdd.Location = new Point(3, 217);
            buttonAdd.Name = "buttonAdd";
            buttonAdd.Size = new Size(90, 66);
            buttonAdd.TabIndex = 19;
            buttonAdd.Text = "+";
            buttonAdd.UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            button3.Dock = DockStyle.Fill;
            button3.Location = new Point(291, 165);
            button3.Name = "button3";
            button3.Size = new Size(90, 46);
            button3.TabIndex = 18;
            button3.Text = "3";
            button3.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            button2.Dock = DockStyle.Fill;
            button2.Location = new Point(195, 165);
            button2.Name = "button2";
            button2.Size = new Size(90, 46);
            button2.TabIndex = 17;
            button2.Text = "2";
            button2.UseVisualStyleBackColor = true;
            // 
            // button1
            // 
            button1.Dock = DockStyle.Fill;
            button1.Location = new Point(99, 165);
            button1.Name = "button1";
            button1.Size = new Size(90, 46);
            button1.TabIndex = 16;
            button1.Text = "1";
            button1.UseVisualStyleBackColor = true;
            // 
            // buttonSubtract
            // 
            buttonSubtract.Dock = DockStyle.Fill;
            buttonSubtract.Location = new Point(3, 165);
            buttonSubtract.Name = "buttonSubtract";
            buttonSubtract.Size = new Size(90, 46);
            buttonSubtract.TabIndex = 15;
            buttonSubtract.Text = "-";
            buttonSubtract.UseVisualStyleBackColor = true;
            // 
            // button6
            // 
            button6.Dock = DockStyle.Fill;
            button6.Location = new Point(291, 114);
            button6.Name = "button6";
            button6.Size = new Size(90, 45);
            button6.TabIndex = 14;
            button6.Text = "6";
            button6.UseVisualStyleBackColor = true;
            // 
            // button5
            // 
            button5.Dock = DockStyle.Fill;
            button5.Location = new Point(195, 114);
            button5.Name = "button5";
            button5.Size = new Size(90, 45);
            button5.TabIndex = 13;
            button5.Text = "5";
            button5.UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            button4.Dock = DockStyle.Fill;
            button4.Location = new Point(99, 114);
            button4.Name = "button4";
            button4.Size = new Size(90, 45);
            button4.TabIndex = 12;
            button4.Text = "4";
            button4.UseVisualStyleBackColor = true;
            // 
            // buttonMultiply
            // 
            buttonMultiply.Dock = DockStyle.Fill;
            buttonMultiply.Location = new Point(3, 114);
            buttonMultiply.Name = "buttonMultiply";
            buttonMultiply.Size = new Size(90, 45);
            buttonMultiply.TabIndex = 11;
            buttonMultiply.Text = "*";
            buttonMultiply.UseVisualStyleBackColor = true;
            // 
            // button9
            // 
            button9.Dock = DockStyle.Fill;
            button9.Location = new Point(291, 64);
            button9.Name = "button9";
            button9.Size = new Size(90, 44);
            button9.TabIndex = 10;
            button9.Text = "9";
            button9.UseVisualStyleBackColor = true;
            // 
            // button8
            // 
            button8.Dock = DockStyle.Fill;
            button8.Location = new Point(195, 64);
            button8.Name = "button8";
            button8.Size = new Size(90, 44);
            button8.TabIndex = 9;
            button8.Text = "8";
            button8.UseVisualStyleBackColor = true;
            // 
            // button7
            // 
            button7.Dock = DockStyle.Fill;
            button7.Location = new Point(99, 64);
            button7.Name = "button7";
            button7.Size = new Size(90, 44);
            button7.TabIndex = 8;
            button7.Text = "7";
            button7.UseVisualStyleBackColor = true;
            // 
            // buttonDivide
            // 
            buttonDivide.Dock = DockStyle.Fill;
            buttonDivide.Location = new Point(3, 64);
            buttonDivide.Name = "buttonDivide";
            buttonDivide.Size = new Size(90, 44);
            buttonDivide.TabIndex = 7;
            buttonDivide.Text = "/";
            buttonDivide.UseVisualStyleBackColor = true;
            // 
            // buttonBackspace
            // 
            buttonBackspace.Dock = DockStyle.Fill;
            buttonBackspace.Location = new Point(291, 30);
            buttonBackspace.Name = "buttonBackspace";
            buttonBackspace.Size = new Size(90, 28);
            buttonBackspace.TabIndex = 6;
            buttonBackspace.Text = "←";
            buttonBackspace.UseVisualStyleBackColor = true;
            // 
            // buttonC
            // 
            buttonC.Dock = DockStyle.Fill;
            buttonC.Location = new Point(99, 30);
            buttonC.Name = "buttonC";
            buttonC.Size = new Size(90, 28);
            buttonC.TabIndex = 5;
            buttonC.Text = "C";
            buttonC.UseVisualStyleBackColor = true;
            // 
            // buttonOpenParenthesis
            // 
            buttonOpenParenthesis.Dock = DockStyle.Fill;
            buttonOpenParenthesis.Location = new Point(3, 30);
            buttonOpenParenthesis.Name = "buttonOpenParenthesis";
            buttonOpenParenthesis.Size = new Size(90, 28);
            buttonOpenParenthesis.TabIndex = 28;
            buttonOpenParenthesis.Text = "(";
            buttonOpenParenthesis.UseVisualStyleBackColor = true;
            // 
            // buttonCloseParenthesis
            // 
            buttonCloseParenthesis.Dock = DockStyle.Fill;
            buttonCloseParenthesis.Location = new Point(195, 30);
            buttonCloseParenthesis.Name = "buttonCloseParenthesis";
            buttonCloseParenthesis.Size = new Size(90, 28);
            buttonCloseParenthesis.TabIndex = 29;
            buttonCloseParenthesis.Text = ")";
            buttonCloseParenthesis.UseVisualStyleBackColor = true;
            // 
            // buttonSqrt
            // 
            buttonSqrt.Dock = DockStyle.Fill;
            buttonSqrt.Location = new Point(3, 3);
            buttonSqrt.Name = "buttonSqrt";
            buttonSqrt.Size = new Size(90, 21);
            buttonSqrt.TabIndex = 24;
            buttonSqrt.Text = "√";
            buttonSqrt.UseVisualStyleBackColor = true;
            // 
            // buttonSquare
            // 
            buttonSquare.Dock = DockStyle.Fill;
            buttonSquare.Location = new Point(99, 3);
            buttonSquare.Name = "buttonSquare";
            buttonSquare.Size = new Size(90, 21);
            buttonSquare.TabIndex = 25;
            buttonSquare.Text = "x²";
            buttonSquare.UseVisualStyleBackColor = true;
            // 
            // buttonPower
            // 
            buttonPower.Dock = DockStyle.Fill;
            buttonPower.Location = new Point(195, 3);
            buttonPower.Name = "buttonPower";
            buttonPower.Size = new Size(90, 21);
            buttonPower.TabIndex = 26;
            buttonPower.Text = "x^y";
            buttonPower.UseVisualStyleBackColor = true;
            // 
            // buttonLog
            // 
            buttonLog.Dock = DockStyle.Fill;
            buttonLog.Location = new Point(291, 3);
            buttonLog.Name = "buttonLog";
            buttonLog.Size = new Size(90, 21);
            buttonLog.TabIndex = 27;
            buttonLog.Text = "log";
            buttonLog.UseVisualStyleBackColor = true;
            // 
            // notifyIcon
            // 
            notifyIcon.ContextMenuStrip = contextMenuStrip;
            notifyIcon.Text = "NumLock Calculator";
            notifyIcon.Visible = true;
            // 
            // numLockTimer
            // 
            numLockTimer.Interval = 500;
            // 
            // CalculatorForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(384, 594);
            Controls.Add(tableLayoutPanel);
            Controls.Add(historyListBox);
            Controls.Add(displayTextBox);
            FormBorderStyle = FormBorderStyle.FixedSingle;
            KeyPreview = true;
            MaximizeBox = false;
            Name = "CalculatorForm";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "Calculator";
            contextMenuStrip.ResumeLayout(false);
            tableLayoutPanel.ResumeLayout(false);
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private System.Windows.Forms.TextBox displayTextBox;
        private System.Windows.Forms.ListBox historyListBox;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel;
        private System.Windows.Forms.Button buttonEquals;
        private System.Windows.Forms.Button buttonDecimal;
        private System.Windows.Forms.Button button0;
        private System.Windows.Forms.Button buttonNegate;
        private System.Windows.Forms.Button buttonAdd;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button buttonSubtract;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Button buttonMultiply;
        private System.Windows.Forms.Button button9;
        private System.Windows.Forms.Button button8;
        private System.Windows.Forms.Button button7;
        private System.Windows.Forms.Button buttonDivide;
        private System.Windows.Forms.Button buttonBackspace;
        private System.Windows.Forms.Button buttonC;
        private System.Windows.Forms.Button buttonSqrt;
        private System.Windows.Forms.Button buttonSquare;
        private System.Windows.Forms.Button buttonPower;
        private System.Windows.Forms.Button buttonLog;
        private System.Windows.Forms.Button buttonOpenParenthesis;
        private System.Windows.Forms.Button buttonCloseParenthesis;
        private System.Windows.Forms.NotifyIcon notifyIcon;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem showMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitMenuItem;
        private System.Windows.Forms.ToolStripMenuItem clearHistoryMenuItem;
        private System.Windows.Forms.Timer numLockTimer;
    }
}
