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
            this.components = new System.ComponentModel.Container();
            this.displayTextBox = new System.Windows.Forms.TextBox();
            this.historyListBox = new System.Windows.Forms.ListBox();
            this.tableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.buttonEquals = new System.Windows.Forms.Button();
            this.buttonDecimal = new System.Windows.Forms.Button();
            this.button0 = new System.Windows.Forms.Button();
            this.buttonNegate = new System.Windows.Forms.Button();
            this.buttonAdd = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.buttonSubtract = new System.Windows.Forms.Button();
            this.button6 = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.buttonMultiply = new System.Windows.Forms.Button();
            this.button9 = new System.Windows.Forms.Button();
            this.button8 = new System.Windows.Forms.Button();
            this.button7 = new System.Windows.Forms.Button();
            this.buttonDivide = new System.Windows.Forms.Button();
            this.buttonBackspace = new System.Windows.Forms.Button();
            this.buttonC = new System.Windows.Forms.Button();
            this.buttonCE = new System.Windows.Forms.Button();
            this.buttonSqrt = new System.Windows.Forms.Button();
            this.buttonSquare = new System.Windows.Forms.Button();
            this.buttonPower = new System.Windows.Forms.Button();
            this.buttonLog = new System.Windows.Forms.Button();
            this.notifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
            this.contextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.showMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.numLockTimer = new System.Windows.Forms.Timer(this.components);
            this.tableLayoutPanel.SuspendLayout();
            this.contextMenuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // displayTextBox
            // 
            this.displayTextBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.displayTextBox.Font = new System.Drawing.Font("Segoe UI", 16F);
            this.displayTextBox.Location = new System.Drawing.Point(0, 0);
            this.displayTextBox.Name = "displayTextBox";
            this.displayTextBox.ReadOnly = true;
            this.displayTextBox.Size = new System.Drawing.Size(384, 36);
            this.displayTextBox.TabIndex = 0;
            this.displayTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // historyListBox
            // 
            this.historyListBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.historyListBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.historyListBox.FormattingEnabled = true;
            this.historyListBox.ItemHeight = 40;
            this.historyListBox.Location = new System.Drawing.Point(0, 36);
            this.historyListBox.Name = "historyListBox";
            this.historyListBox.ScrollAlwaysVisible = true;
            this.historyListBox.Size = new System.Drawing.Size(384, 154);
            this.historyListBox.TabIndex = 1;
            this.historyListBox.DoubleClick += new System.EventHandler(this.historyListBox_DoubleClick);
            this.historyListBox.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.historyListBox_DrawItem);
            // 
            // tableLayoutPanel
            // 
            this.tableLayoutPanel.ColumnCount = 4;
            this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel.Controls.Add(this.buttonEquals, 3, 5);
            this.tableLayoutPanel.Controls.Add(this.buttonDecimal, 2, 5);
            this.tableLayoutPanel.Controls.Add(this.button0, 1, 5);
            this.tableLayoutPanel.Controls.Add(this.buttonNegate, 0, 5);
            this.tableLayoutPanel.Controls.Add(this.buttonAdd, 3, 4);
            this.tableLayoutPanel.Controls.Add(this.button3, 2, 4);
            this.tableLayoutPanel.Controls.Add(this.button2, 1, 4);
            this.tableLayoutPanel.Controls.Add(this.button1, 0, 4);
            this.tableLayoutPanel.Controls.Add(this.buttonSubtract, 3, 3);
            this.tableLayoutPanel.Controls.Add(this.button6, 2, 3);
            this.tableLayoutPanel.Controls.Add(this.button5, 1, 3);
            this.tableLayoutPanel.Controls.Add(this.button4, 0, 3);
            this.tableLayoutPanel.Controls.Add(this.buttonMultiply, 3, 2);
            this.tableLayoutPanel.Controls.Add(this.button9, 2, 2);
            this.tableLayoutPanel.Controls.Add(this.button8, 1, 2);
            this.tableLayoutPanel.Controls.Add(this.button7, 0, 2);
            this.tableLayoutPanel.Controls.Add(this.buttonDivide, 3, 1);
            this.tableLayoutPanel.Controls.Add(this.buttonBackspace, 2, 1);
            this.tableLayoutPanel.Controls.Add(this.buttonC, 1, 1);
            this.tableLayoutPanel.Controls.Add(this.buttonCE, 0, 1);
            this.tableLayoutPanel.Controls.Add(this.buttonSqrt, 0, 0);
            this.tableLayoutPanel.Controls.Add(this.buttonSquare, 1, 0);
            this.tableLayoutPanel.Controls.Add(this.buttonPower, 2, 0);
            this.tableLayoutPanel.Controls.Add(this.buttonLog, 3, 0);
            this.tableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel.Location = new System.Drawing.Point(0, 190);
            this.tableLayoutPanel.Name = "tableLayoutPanel";
            this.tableLayoutPanel.RowCount = 6;
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.tableLayoutPanel.Size = new System.Drawing.Size(384, 371);
            this.tableLayoutPanel.TabIndex = 2;
            // 
            // buttonEquals
            // 
            this.buttonEquals.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonEquals.Location = new System.Drawing.Point(291, 308);
            this.buttonEquals.Name = "buttonEquals";
            this.buttonEquals.Size = new System.Drawing.Size(90, 60);
            this.buttonEquals.TabIndex = 23;
            this.buttonEquals.Text = "=";
            this.buttonEquals.UseVisualStyleBackColor = true;
            // 
            // buttonDecimal
            // 
            this.buttonDecimal.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonDecimal.Location = new System.Drawing.Point(195, 308);
            this.buttonDecimal.Name = "buttonDecimal";
            this.buttonDecimal.Size = new System.Drawing.Size(90, 60);
            this.buttonDecimal.TabIndex = 22;
            this.buttonDecimal.Text = ".";
            this.buttonDecimal.UseVisualStyleBackColor = true;
            // 
            // button0
            // 
            this.button0.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button0.Location = new System.Drawing.Point(99, 308);
            this.button0.Name = "button0";
            this.button0.Size = new System.Drawing.Size(90, 60);
            this.button0.TabIndex = 21;
            this.button0.Text = "0";
            this.button0.UseVisualStyleBackColor = true;
            // 
            // buttonNegate
            // 
            this.buttonNegate.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonNegate.Location = new System.Drawing.Point(3, 308);
            this.buttonNegate.Name = "buttonNegate";
            this.buttonNegate.Size = new System.Drawing.Size(90, 60);
            this.buttonNegate.TabIndex = 20;
            this.buttonNegate.Text = "+/-";
            this.buttonNegate.UseVisualStyleBackColor = true;
            // 
            // buttonAdd
            // 
            this.buttonAdd.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonAdd.Location = new System.Drawing.Point(291, 247);
            this.buttonAdd.Name = "buttonAdd";
            this.buttonAdd.Size = new System.Drawing.Size(90, 55);
            this.buttonAdd.TabIndex = 19;
            this.buttonAdd.Text = "+";
            this.buttonAdd.UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            this.button3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button3.Location = new System.Drawing.Point(195, 247);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(90, 55);
            this.button3.TabIndex = 18;
            this.button3.Text = "3";
            this.button3.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this.button2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button2.Location = new System.Drawing.Point(99, 247);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(90, 55);
            this.button2.TabIndex = 17;
            this.button2.Text = "2";
            this.button2.UseVisualStyleBackColor = true;
            // 
            // button1
            // 
            this.button1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button1.Location = new System.Drawing.Point(3, 247);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(90, 55);
            this.button1.TabIndex = 16;
            this.button1.Text = "1";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // buttonSubtract
            // 
            this.buttonSubtract.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonSubtract.Location = new System.Drawing.Point(291, 186);
            this.buttonSubtract.Name = "buttonSubtract";
            this.buttonSubtract.Size = new System.Drawing.Size(90, 55);
            this.buttonSubtract.TabIndex = 15;
            this.buttonSubtract.Text = "-";
            this.buttonSubtract.UseVisualStyleBackColor = true;
            // 
            // button6
            // 
            this.button6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button6.Location = new System.Drawing.Point(195, 186);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(90, 55);
            this.button6.TabIndex = 14;
            this.button6.Text = "6";
            this.button6.UseVisualStyleBackColor = true;
            // 
            // button5
            // 
            this.button5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button5.Location = new System.Drawing.Point(99, 186);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(90, 55);
            this.button5.TabIndex = 13;
            this.button5.Text = "5";
            this.button5.UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            this.button4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button4.Location = new System.Drawing.Point(3, 186);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(90, 55);
            this.button4.TabIndex = 12;
            this.button4.Text = "4";
            this.button4.UseVisualStyleBackColor = true;
            // 
            // buttonMultiply
            // 
            this.buttonMultiply.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonMultiply.Location = new System.Drawing.Point(291, 125);
            this.buttonMultiply.Name = "buttonMultiply";
            this.buttonMultiply.Size = new System.Drawing.Size(90, 55);
            this.buttonMultiply.TabIndex = 11;
            this.buttonMultiply.Text = "*";
            this.buttonMultiply.UseVisualStyleBackColor = true;
            // 
            // button9
            // 
            this.button9.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button9.Location = new System.Drawing.Point(195, 125);
            this.button9.Name = "button9";
            this.button9.Size = new System.Drawing.Size(90, 55);
            this.button9.TabIndex = 10;
            this.button9.Text = "9";
            this.button9.UseVisualStyleBackColor = true;
            // 
            // button8
            // 
            this.button8.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button8.Location = new System.Drawing.Point(99, 125);
            this.button8.Name = "button8";
            this.button8.Size = new System.Drawing.Size(90, 55);
            this.button8.TabIndex = 9;
            this.button8.Text = "8";
            this.button8.UseVisualStyleBackColor = true;
            // 
            // button7
            // 
            this.button7.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button7.Location = new System.Drawing.Point(3, 125);
            this.button7.Name = "button7";
            this.button7.Size = new System.Drawing.Size(90, 55);
            this.button7.TabIndex = 8;
            this.button7.Text = "7";
            this.button7.UseVisualStyleBackColor = true;
            // 
            // buttonDivide
            // 
            this.buttonDivide.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonDivide.Location = new System.Drawing.Point(291, 64);
            this.buttonDivide.Name = "buttonDivide";
            this.buttonDivide.Size = new System.Drawing.Size(90, 55);
            this.buttonDivide.TabIndex = 7;
            this.buttonDivide.Text = "/";
            this.buttonDivide.UseVisualStyleBackColor = true;
            // 
            // buttonBackspace
            // 
            this.buttonBackspace.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonBackspace.Location = new System.Drawing.Point(195, 64);
            this.buttonBackspace.Name = "buttonBackspace";
            this.buttonBackspace.Size = new System.Drawing.Size(90, 55);
            this.buttonBackspace.TabIndex = 6;
            this.buttonBackspace.Text = "←";
            this.buttonBackspace.UseVisualStyleBackColor = true;
            // 
            // buttonC
            // 
            this.buttonC.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonC.Location = new System.Drawing.Point(99, 64);
            this.buttonC.Name = "buttonC";
            this.buttonC.Size = new System.Drawing.Size(90, 55);
            this.buttonC.TabIndex = 5;
            this.buttonC.Text = "C";
            this.buttonC.UseVisualStyleBackColor = true;
            // 
            // buttonCE
            // 
            this.buttonCE.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonCE.Location = new System.Drawing.Point(3, 64);
            this.buttonCE.Name = "buttonCE";
            this.buttonCE.Size = new System.Drawing.Size(90, 55);
            this.buttonCE.TabIndex = 4;
            this.buttonCE.Text = "CE";
            this.buttonCE.UseVisualStyleBackColor = true;
            // 
            // buttonSqrt
            // 
            this.buttonSqrt.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonSqrt.Location = new System.Drawing.Point(3, 3);
            this.buttonSqrt.Name = "buttonSqrt";
            this.buttonSqrt.Size = new System.Drawing.Size(90, 55);
            this.buttonSqrt.TabIndex = 24;
            this.buttonSqrt.Text = "√";
            this.buttonSqrt.UseVisualStyleBackColor = true;
            // 
            // buttonSquare
            // 
            this.buttonSquare.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonSquare.Location = new System.Drawing.Point(99, 3);
            this.buttonSquare.Name = "buttonSquare";
            this.buttonSquare.Size = new System.Drawing.Size(90, 55);
            this.buttonSquare.TabIndex = 25;
            this.buttonSquare.Text = "x²";
            this.buttonSquare.UseVisualStyleBackColor = true;
            // 
            // buttonPower
            // 
            this.buttonPower.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonPower.Location = new System.Drawing.Point(195, 3);
            this.buttonPower.Name = "buttonPower";
            this.buttonPower.Size = new System.Drawing.Size(90, 55);
            this.buttonPower.TabIndex = 26;
            this.buttonPower.Text = "x^y";
            this.buttonPower.UseVisualStyleBackColor = true;
            // 
            // buttonLog
            // 
            this.buttonLog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.buttonLog.Location = new System.Drawing.Point(291, 3);
            this.buttonLog.Name = "buttonLog";
            this.buttonLog.Size = new System.Drawing.Size(90, 55);
            this.buttonLog.TabIndex = 27;
            this.buttonLog.Text = "log";
            this.buttonLog.UseVisualStyleBackColor = true;
            // 
            // notifyIcon
            // 
            this.notifyIcon.ContextMenuStrip = this.contextMenuStrip;
            this.notifyIcon.Text = "NumLock Calculator";
            this.notifyIcon.Visible = true;
            // 
            // contextMenuStrip
            // 
            this.contextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showMenuItem,
            this.exitMenuItem});
            this.contextMenuStrip.Name = "contextMenuStrip";
            this.contextMenuStrip.Size = new System.Drawing.Size(161, 48);
            // 
            // showMenuItem
            // 
            this.showMenuItem.Name = "showMenuItem";
            this.showMenuItem.Size = new System.Drawing.Size(160, 22);
            this.showMenuItem.Text = "Show Calculator";
            // 
            // exitMenuItem
            // 
            this.exitMenuItem.Name = "exitMenuItem";
            this.exitMenuItem.Size = new System.Drawing.Size(160, 22);
            this.exitMenuItem.Text = "Exit";
            // 
            // numLockTimer
            // 
            this.numLockTimer.Interval = 500;
            // 
            // CalculatorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(384, 561);
            this.Controls.Add(this.tableLayoutPanel);
            this.Controls.Add(this.historyListBox);
            this.Controls.Add(this.displayTextBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.KeyPreview = true;
            this.MaximizeBox = false;
            this.Name = "CalculatorForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Calculator";
            this.tableLayoutPanel.ResumeLayout(false);
            this.contextMenuStrip.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();
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
        private System.Windows.Forms.Button buttonCE;
        private System.Windows.Forms.Button buttonSqrt;
        private System.Windows.Forms.Button buttonSquare;
        private System.Windows.Forms.Button buttonPower;
        private System.Windows.Forms.Button buttonLog;
        private System.Windows.Forms.NotifyIcon notifyIcon;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem showMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitMenuItem;
        private System.Windows.Forms.Timer numLockTimer;
    }
}