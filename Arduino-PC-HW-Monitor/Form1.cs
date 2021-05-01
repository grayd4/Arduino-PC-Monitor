using OpenHardwareMonitor.Hardware;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Arduino_PC_HW_Monitor
{
    public partial class Form1 : Form
    {
        static string data;
        Computer c = new Computer()
        {
            GPUEnabled = true,
            CPUEnabled = true,
            RAMEnabled = true
        };

        int GPUTemp, CPUTemp, CPULoad, RAMLoad, RAMUsed;

        private SerialPort port = new SerialPort();
        public Form1()
        {
            InitializeComponent();
            Init();
            c.Open();
        }


        private void Init()
        {
            try
            {
                notifyIcon1.Visible = false;
                port.Parity = Parity.None;
                port.StopBits = StopBits.One;
                port.DataBits = 8;
                port.Handshake = Handshake.None;
                port.RtsEnable = true;
                string[] ports = SerialPort.GetPortNames();
                foreach (string port in ports)
                {
                    comboBoxPort.Items.Add(port);
                }
                comboBoxPort.SelectedItem = "COM4";
                port.BaudRate = 9600;
                comboBoxInterval.SelectedItem = "100";

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        private void buttonStop_Click(object sender, EventArgs e)
        {
            try
            {
                port.Write("DIS*");
                port.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            labelStatus.Text = "Disconnected";
            timerUpdate.Enabled = false;
            toolStripStatusLabel1.Text = "Connect to Arduino...";
            //data = "";
        }


        private void buttonGo_Click(object sender, EventArgs e)
        {
            try
            {
                if (!port.IsOpen)
                {
                    port.PortName = comboBoxPort.Text;
                    port.Open();
                    timerUpdate.Interval = Convert.ToInt32(comboBoxInterval.Text);
                    timerUpdate.Enabled = true;
                    toolStripStatusLabel1.Text = "Sending data...";
                    labelStatus.Text = "Connected";
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        //private void timer1_Tick(object sender, EventArgs e)
        //{
        //    Status();
        //}

        private void timerUpdate_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            Status();
        }

        //private void Form1_Load(object sender, EventArgs e)
        //{
        //c.Open();
        //}

        private void Form1_Resize(object sender, EventArgs e)
        {
            if (FormWindowState.Minimized == this.WindowState)
            {
                notifyIcon1.Visible = true;
                try
                {
                    notifyIcon1.ShowBalloonTip(500, "Arduino", toolStripStatusLabel1.Text, ToolTipIcon.Info);

                }
                catch (Exception ex)
                {

                }
                this.Hide();
            }


        }


        private void notifyIcon1_DoubleClick(object sender, EventArgs e)
        {

            this.Show();
            this.WindowState = FormWindowState.Normal;
            notifyIcon1.Visible = false;
        }

        

        private void Status()
        {
            foreach (var hardware in c.Hardware)
            {

                if (hardware.HardwareType == HardwareType.GpuNvidia)
                {
                    hardware.Update();
                    foreach (var sensor in hardware.Sensors)
                        if (sensor.SensorType == SensorType.Temperature)
                        {
                            GPUTemp = (int) sensor.Value.GetValueOrDefault();
                            System.Diagnostics.Debug.WriteLine("GPUTemp: " + sensor.Value.GetValueOrDefault());
                        }

                }

                if (hardware.HardwareType == HardwareType.CPU)
                {
                    hardware.Update();
                    foreach (var sensor in hardware.Sensors)
                    {
                        if (sensor.SensorType == SensorType.Temperature && sensor.Name.Contains("Package"))
                        {
                            CPUTemp = (int) sensor.Value.GetValueOrDefault();
                            System.Diagnostics.Debug.WriteLine("CPUTemp: " + CPUTemp);
                        }
                        if (sensor.SensorType == SensorType.Load && sensor.Name.Contains("Total"))
                        {
                            CPULoad = (int) sensor.Value.GetValueOrDefault();
                            System.Diagnostics.Debug.WriteLine("CPULoad: " + CPULoad);
                        }
                    }
                }
                if (hardware.HardwareType == HardwareType.RAM)
                {
                    hardware.Update();
                    foreach (var sensor in hardware.Sensors)
                    {
                        if (sensor.SensorType == SensorType.Load && sensor.Name.Contains("Memory"))
                        {
                            RAMLoad = (int) sensor.Value.GetValueOrDefault();
                            System.Diagnostics.Debug.WriteLine("RAMLoad: " + RAMLoad);

                        }
                        if (sensor.SensorType == SensorType.Data && sensor.Name.Contains("Used Memory"))
                        {
                            RAMUsed = (int) sensor.Value.GetValueOrDefault();
                            System.Diagnostics.Debug.WriteLine("RAMUsed: " + RAMUsed);
                        }
                    }   
                }


            }
            try
            {
                string output = CPULoad + "*" + CPUTemp + "#" + RAMLoad + "$" + RAMUsed + "&";
                port.Write(output);
                System.Diagnostics.Debug.WriteLine("combined: " + output);
            }
            catch (Exception ex)
            {
                timerUpdate.Stop();
                MessageBox.Show(ex.Message);
                toolStripStatusLabel1.Text = "Arduino's not responding...";
            }
        }

    }
}
