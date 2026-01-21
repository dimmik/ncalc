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
            this.buttonsTableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.equalsPanel = new System.Windows.Forms.Panel();
            displayTextBox = new TextBox();
            historyListBox = new ListBox();
            contextMenuStrip = new ContextMenuStrip(components);
            showMenuItem = new ToolStripMenuItem();
            exitMenuItem = new ToolStripMenuItem();
            clearHistoryMenuItem = new ToolStripMenuItem();
            
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
            this.buttonsTableLayoutPanel.SuspendLayout();
            SuspendLayout();
            // 
            // displayTextBox
            // 
            displayTextBox.Dock = DockStyle.Top;
            displayTextBox.Font = new Font("Segoe UI", 16F);
            displayTextBox.Location = new Point(0, 0);
            displayTextBox.Margin = new Padding(5, 6, 5, 6);
            displayTextBox.Name = "displayTextBox";
            displayTextBox.ReadOnly = true;
            displayTextBox.Size = new Size(658, 57);
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
            historyListBox.Location = new Point(0, 57);
            historyListBox.Margin = new Padding(5, 6, 5, 6);
            historyListBox.Name = "historyListBox";
            historyListBox.ScrollAlwaysVisible = true;
            historyListBox.Size = new Size(658, 304);
            historyListBox.TabIndex = 1;
            historyListBox.DrawItem += historyListBox_DrawItem;
            historyListBox.DoubleClick += historyListBox_DoubleClick;
            // 
            // contextMenuStrip
            // 
            contextMenuStrip.ImageScalingSize = new Size(28, 28);
            contextMenuStrip.Items.AddRange(new ToolStripItem[] { showMenuItem, exitMenuItem, clearHistoryMenuItem });
            contextMenuStrip.Name = "contextMenuStrip";
            contextMenuStrip.Size = new Size(236, 112);
            // 
            // showMenuItem
            // 
            showMenuItem.Name = "showMenuItem";
            showMenuItem.Size = new Size(235, 36);
            showMenuItem.Text = "Show Calculator";
            // 
            // exitMenuItem
            // 
            exitMenuItem.Name = "exitMenuItem";
            exitMenuItem.Size = new Size(235, 36);
            exitMenuItem.Text = "Exit";
            // 
            // clearHistoryMenuItem
            // 
            clearHistoryMenuItem.Name = "clearHistoryMenuItem";
            clearHistoryMenuItem.Size = new Size(235, 36);
            clearHistoryMenuItem.Text = "Clear History";
            //
            // buttonsTableLayoutPanel
            //
            this.buttonsTableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.buttonsTableLayoutPanel.ColumnCount = 5;
            this.buttonsTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.buttonsTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.buttonsTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.buttonsTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.buttonsTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonLog, 0, 0);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonSqrt, 1, 0);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonSquare, 2, 0);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonPower, 3, 0);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonDivide, 4, 0);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonOpenParenthesis, 0, 1);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonCloseParenthesis, 1, 1);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonC, 2, 1);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonBackspace, 3, 1);
            this.buttonsTableLayoutPanel.Controls.Add(this.button7, 0, 2);
            this.buttonsTableLayoutPanel.Controls.Add(this.button8, 1, 2);
            this.buttonsTableLayoutPanel.Controls.Add(this.button9, 2, 2);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonMultiply, 3, 2);
            this.buttonsTableLayoutPanel.Controls.Add(this.button4, 0, 3);
            this.buttonsTableLayoutPanel.Controls.Add(this.button5, 1, 3);
            this.buttonsTableLayoutPanel.Controls.Add(this.button6, 2, 3);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonSubtract, 3, 3);
            this.buttonsTableLayoutPanel.Controls.Add(this.button1, 0, 4);
            this.buttonsTableLayoutPanel.Controls.Add(this.button2, 1, 4);
            this.buttonsTableLayoutPanel.Controls.Add(this.button3, 2, 4);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonAdd, 3, 4);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonNegate, 0, 5);
            this.buttonsTableLayoutPanel.Controls.Add(this.button0, 1, 5);
            this.buttonsTableLayoutPanel.Controls.Add(this.buttonDecimal, 2, 5);
            this.buttonsTableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonsTableLayoutPanel.Location = new System.Drawing.Point(0, 361);
            this.buttonsTableLayoutPanel.Margin = new System.Windows.Forms.Padding(5, 6, 5, 6);
            this.buttonsTableLayoutPanel.Name = "buttonsTableLayoutPanel";
            this.buttonsTableLayoutPanel.RowCount = 6;
            this.buttonsTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.buttonsTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.buttonsTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.buttonsTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.buttonsTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.buttonsTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.buttonsTableLayoutPanel.Size = new System.Drawing.Size(658, 680);
            this.buttonsTableLayoutPanel.TabIndex = 2;
            //
            // equalsPanel
            //
            this.equalsPanel = new System.Windows.Forms.Panel();
            this.equalsPanel.Controls.Add(this.buttonEquals);
            this.equalsPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.equalsPanel.Location = new System.Drawing.Point(0, 1041);
            this.equalsPanel.Name = "equalsPanel";
            this.equalsPanel.Size = new System.Drawing.Size(658, 81);
            this.equalsPanel.TabIndex = 3;
            // 
            // buttonEquals
            // 
            buttonEquals.Dock = DockStyle.Fill;
            buttonEquals.Location = new Point(0, 0);
            buttonEquals.Name = "buttonEquals";
            buttonEquals.Size = new Size(658, 81);
            buttonEquals.TabIndex = 0;
            buttonEquals.Text = "=";
            buttonEquals.UseVisualStyleBackColor = true;
            buttonEquals.Click += EqualsClick;
            this.buttonEquals.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonDecimal
            // 
            buttonDecimal.Dock = DockStyle.Fill;
            buttonDecimal.Location = new Point(497, 621);
            buttonDecimal.Margin = new Padding(5, 6, 5, 6);
            buttonDecimal.Name = "buttonDecimal";
            buttonDecimal.Size = new Size(156, 111);
            buttonDecimal.TabIndex = 22;
            buttonDecimal.Text = ".";
            buttonDecimal.UseVisualStyleBackColor = true;
            this.buttonDecimal.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button0
            // 
            button0.Dock = DockStyle.Fill;
            button0.Location = new Point(333, 621);
            button0.Margin = new Padding(5, 6, 5, 6);
            button0.Name = "button0";
            button0.Size = new Size(154, 111);
            button0.TabIndex = 21;
            button0.Text = "0";
            button0.UseVisualStyleBackColor = true;
            this.button0.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonNegate
            // 
            buttonNegate.Dock = DockStyle.Fill;
            buttonNegate.Location = new Point(169, 621);
            buttonNegate.Margin = new Padding(5, 6, 5, 6);
            buttonNegate.Name = "buttonNegate";
            buttonNegate.Size = new Size(154, 111);
            buttonNegate.TabIndex = 20;
            buttonNegate.Text = "+/-";
            buttonNegate.UseVisualStyleBackColor = true;
            this.buttonNegate.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonAdd
            // 
            buttonAdd.Dock = DockStyle.Fill;
            buttonAdd.Location = new Point(5, 621);
            buttonAdd.Margin = new Padding(5, 6, 5, 6);
            buttonAdd.Name = "buttonAdd";
            buttonAdd.Size = new Size(154, 111);
            buttonAdd.TabIndex = 19;
            buttonAdd.Text = "+";
            buttonAdd.UseVisualStyleBackColor = true;
            this.buttonAdd.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button3
            // 
            button3.Dock = DockStyle.Fill;
            button3.Location = new Point(497, 498);
            button3.Margin = new Padding(5, 6, 5, 6);
            button3.Name = "button3";
            button3.Size = new Size(156, 111);
            button3.TabIndex = 18;
            button3.Text = "3";
            button3.UseVisualStyleBackColor = true;
            this.button3.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button2
            // 
            button2.Dock = DockStyle.Fill;
            button2.Location = new Point(333, 498);
            button2.Margin = new Padding(5, 6, 5, 6);
            button2.Name = "button2";
            button2.Size = new Size(154, 111);
            button2.TabIndex = 17;
            button2.Text = "2";
            button2.UseVisualStyleBackColor = true;
            this.button2.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button1
            // 
            button1.Dock = DockStyle.Fill;
            button1.Location = new Point(169, 498);
            button1.Margin = new Padding(5, 6, 5, 6);
            button1.Name = "button1";
            button1.Size = new Size(154, 111);
            button1.TabIndex = 16;
            button1.Text = "1";
            button1.UseVisualStyleBackColor = true;
            this.button1.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonSubtract
            // 
            buttonSubtract.Dock = DockStyle.Fill;
            buttonSubtract.Location = new Point(5, 498);
            buttonSubtract.Margin = new Padding(5, 6, 5, 6);
            buttonSubtract.Name = "buttonSubtract";
            buttonSubtract.Size = new Size(154, 111);
            buttonSubtract.TabIndex = 15;
            buttonSubtract.Text = "-";
            buttonSubtract.UseVisualStyleBackColor = true;
            this.buttonSubtract.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button6
            // 
            button6.Dock = DockStyle.Fill;
            button6.Location = new Point(497, 375);
            button6.Margin = new Padding(5, 6, 5, 6);
            button6.Name = "button6";
            button6.Size = new Size(156, 111);
            button6.TabIndex = 14;
            button6.Text = "6";
            button6.UseVisualStyleBackColor = true;
            this.button6.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button5
            // 
            button5.Dock = DockStyle.Fill;
            button5.Location = new Point(333, 375);
            button5.Margin = new Padding(5, 6, 5, 6);
            button5.Name = "button5";
            button5.Size = new Size(154, 111);
            button5.TabIndex = 13;
            button5.Text = "5";
            button5.UseVisualStyleBackColor = true;
            this.button5.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button4
            // 
            button4.Dock = DockStyle.Fill;
            button4.Location = new Point(169, 375);
            button4.Margin = new Padding(5, 6, 5, 6);
            button4.Name = "button4";
            button4.Size = new Size(154, 111);
            button4.TabIndex = 12;
            button4.Text = "4";
            button4.UseVisualStyleBackColor = true;
            this.button4.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonMultiply
            // 
            buttonMultiply.Dock = DockStyle.Fill;
            buttonMultiply.Location = new Point(5, 375);
            buttonMultiply.Margin = new Padding(5, 6, 5, 6);
            buttonMultiply.Name = "buttonMultiply";
            buttonMultiply.Size = new Size(154, 111);
            buttonMultiply.TabIndex = 11;
            buttonMultiply.Text = "*";
            buttonMultiply.UseVisualStyleBackColor = true;
            this.buttonMultiply.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button9
            // 
            button9.Dock = DockStyle.Fill;
            button9.Location = new Point(497, 252);
            button9.Margin = new Padding(5, 6, 5, 6);
            button9.Name = "button9";
            button9.Size = new Size(156, 111);
            button9.TabIndex = 10;
            button9.Text = "9";
            button9.UseVisualStyleBackColor = true;
            this.button9.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button8
            // 
            button8.Dock = DockStyle.Fill;
            button8.Location = new Point(333, 252);
            button8.Margin = new Padding(5, 6, 5, 6);
            button8.Name = "button8";
            button8.Size = new Size(154, 111);
            button8.TabIndex = 9;
            button8.Text = "8";
            button8.UseVisualStyleBackColor = true;
            this.button8.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // button7
            // 
            button7.Dock = DockStyle.Fill;
            button7.Location = new Point(169, 252);
            button7.Margin = new Padding(5, 6, 5, 6);
            button7.Name = "button7";
            button7.Size = new Size(154, 111);
            button7.TabIndex = 8;
            button7.Text = "7";
            button7.UseVisualStyleBackColor = true;
            this.button7.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonDivide
            // 
            buttonDivide.Dock = DockStyle.Fill;
            buttonDivide.Location = new Point(5, 252);
            buttonDivide.Margin = new Padding(5, 6, 5, 6);
            buttonDivide.Name = "buttonDivide";
            buttonDivide.Size = new Size(154, 111);
            buttonDivide.TabIndex = 7;
            buttonDivide.Text = "/";
            buttonDivide.UseVisualStyleBackColor = true;
            this.buttonDivide.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonBackspace
            // 
            buttonBackspace.Dock = DockStyle.Fill;
            buttonBackspace.Location = new Point(497, 129);
            buttonBackspace.Margin = new Padding(5, 6, 5, 6);
            buttonBackspace.Name = "buttonBackspace";
            buttonBackspace.Size = new Size(156, 111);
            buttonBackspace.TabIndex = 6;
            buttonBackspace.Text = "←";
            buttonBackspace.UseVisualStyleBackColor = true;
            this.buttonBackspace.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonC
            // 
            buttonC.Dock = DockStyle.Fill;
            buttonC.Location = new Point(169, 129);
            buttonC.Margin = new Padding(5, 6, 5, 6);
            buttonC.Name = "buttonC";
            buttonC.Size = new Size(154, 111);
            buttonC.TabIndex = 5;
            buttonC.Text = "C";
            buttonC.UseVisualStyleBackColor = true;
            this.buttonC.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonOpenParenthesis
            // 
            buttonOpenParenthesis.Dock = DockStyle.Fill;
            buttonOpenParenthesis.Location = new Point(5, 129);
            buttonOpenParenthesis.Margin = new Padding(5, 6, 5, 6);
            buttonOpenParenthesis.Name = "buttonOpenParenthesis";
            buttonOpenParenthesis.Size = new Size(154, 111);
            buttonOpenParenthesis.TabIndex = 28;
            buttonOpenParenthesis.Text = "(";
            buttonOpenParenthesis.UseVisualStyleBackColor = true;
            this.buttonOpenParenthesis.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonCloseParenthesis
            // 
            buttonCloseParenthesis.Dock = DockStyle.Fill;
            buttonCloseParenthesis.Location = new Point(333, 129);
            buttonCloseParenthesis.Margin = new Padding(5, 6, 5, 6);
            buttonCloseParenthesis.Name = "buttonCloseParenthesis";
            buttonCloseParenthesis.Size = new Size(154, 111);
            buttonCloseParenthesis.TabIndex = 29;
            buttonCloseParenthesis.Text = ")";
            buttonCloseParenthesis.UseVisualStyleBackColor = true;
            this.buttonCloseParenthesis.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonSqrt
            // 
            buttonSqrt.Dock = DockStyle.Fill;
            buttonSqrt.Location = new Point(5, 6);
            buttonSqrt.Margin = new Padding(5, 6, 5, 6);
            buttonSqrt.Name = "buttonSqrt";
            buttonSqrt.Size = new Size(154, 111);
            buttonSqrt.TabIndex = 24;
            buttonSqrt.Text = "√";
            buttonSqrt.UseVisualStyleBackColor = true;
            this.buttonSqrt.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonSquare
            // 
            buttonSquare.Dock = DockStyle.Fill;
            buttonSquare.Location = new Point(169, 6);
            buttonSquare.Margin = new Padding(5, 6, 5, 6);
            buttonSquare.Name = "buttonSquare";
            buttonSquare.Size = new Size(154, 111);
            buttonSquare.TabIndex = 25;
            buttonSquare.Text = "x²";
            buttonSquare.UseVisualStyleBackColor = true;
            this.buttonSquare.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonPower
            // 
            buttonPower.Dock = DockStyle.Fill;
            buttonPower.Location = new Point(333, 6);
            buttonPower.Margin = new Padding(5, 6, 5, 6);
            buttonPower.Name = "buttonPower";
            buttonPower.Size = new Size(154, 111);
            buttonPower.TabIndex = 26;
            buttonPower.Text = "x^y";
            buttonPower.UseVisualStyleBackColor = true;
            this.buttonPower.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
            // 
            // buttonLog
            // 
            buttonLog.Dock = DockStyle.Fill;
            buttonLog.Location = new Point(497, 6);
            buttonLog.Margin = new Padding(5, 6, 5, 6);
            buttonLog.Name = "buttonLog";
            buttonLog.Size = new Size(156, 111);
            buttonLog.TabIndex = 27;
            buttonLog.Text = "log";
            buttonLog.UseVisualStyleBackColor = true;
            this.buttonLog.Padding = new System.Windows.Forms.Padding(0, 10, 0, 10);
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
            AutoScaleDimensions = new SizeF(12F, 30F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(658, 1122);
            Controls.Add(this.buttonsTableLayoutPanel);
            Controls.Add(this.equalsPanel);
            Controls.Add(historyListBox);
            Controls.Add(displayTextBox);
            FormBorderStyle = FormBorderStyle.FixedSingle;
            KeyPreview = true;
            Margin = new Padding(5, 6, 5, 6);
            MaximizeBox = false;
            Name = "CalculatorForm";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "Calculator";
            contextMenuStrip.ResumeLayout(false);
            this.buttonsTableLayoutPanel.ResumeLayout(false);
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private System.Windows.Forms.TextBox displayTextBox;
        private System.Windows.Forms.ListBox historyListBox;
        private System.Windows.Forms.TableLayoutPanel buttonsTableLayoutPanel;
        private System.Windows.Forms.Panel equalsPanel;
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
