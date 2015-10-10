
/** @file $Id: WinRobotDemo.java 60 2011-06-06 15:53:27Z caoym $
 *  @author caoym
 *  @brief WinRobotDemo			
 */
import java.awt.AWTException;
import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.image.BufferedImage;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.nio.channels.FileChannel;
import java.util.Timer;
import java.util.TimerTask;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import com.caoym.WinFileMappingBuffer;
import com.caoym.WinRobot;

/**
 * a simple test for WinRobot
 * @author caoym 
 *
 */
public class WinRobotDemo extends JPanel {

    /**
     * 
     */
    private static final long serialVersionUID = 1L;

    public WinRobotDemo(WinRobot robot) {
        _lastCursorPos = new Rectangle();
        _robot = robot;
        // a timer for repaint 
        new Timer().schedule(new TimerTask() {

            @Override
            public void run() {
                try {
                    SwingUtilities.invokeAndWait(new Runnable() {

                        @Override
                        public void run() {
                            repaint();
                        }
                    });
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }, 100, 100);

    }
    WinRobot _robot;
    Rectangle _lastCursorPos;

    /**
     *  these codes only work on 32bits desktop
     */
    @Override
    public void paint(Graphics g) {
        super.setBackground(new Color(255,0,0,255));
        super.paint(g);
        //if(true)return ;
        Graphics2D g2d = (Graphics2D) g;
        //Composite oriComposite = g2d.getComposite();
        // draw screen image
        BufferedImage screen = _robot.createScreenCapture(new Rectangle(-65535, -65535, 65535 * 2, 65535 * 2));
        if (screen == null) {
            return;
        }
        
       g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0f));
       
        g2d.drawImage(screen, 0, 0, 400, 300, new Color(255, 255, 255, 255), this);

        // draw cursor
        Point ori = new Point();
        Point hotspot = new Point();
        int type[] = {0};
        BufferedImage cursor = _robot.captureMouseCursor(ori, hotspot,type);
        if (cursor == null) {
            return;
        }
        g.clipRect(0, 300, 400, 300);


        _lastCursorPos.x = ori.x * 400 / screen.getWidth() - hotspot.x;
        _lastCursorPos.y = 300 + ori.y * 300 / screen.getHeight() - hotspot.y;
        _lastCursorPos.width = cursor.getWidth();
        _lastCursorPos.height = cursor.getHeight();

        g2d.drawImage(cursor, _lastCursorPos.x, _lastCursorPos.y, this);
        
        //screen = null;
    }

    public static void InputChars(WinRobot bot, String str) {
        for (int i = 0; i < str.length(); i++) {
            bot.keyType(str.charAt(i));
        }

    }

    /**
     * @param args
     * @throws AWTException 
     * @throws IOException 
     */
    public static void main(String[] args) throws AWTException {

        GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
        GraphicsDevice[] gs = ge.getScreenDevices();

        class Item {

            @Override
            public String toString() {
                if (dev == null) {
                    return "";
                }
                return dev.getIDstring();
            }
            public GraphicsDevice dev;
        }
        Item items[] = new Item[gs.length];
        for (int i = 0; i < gs.length; i++) {
            items[i] = new Item();
            items[i].dev = gs[i];
        }
        Object selectedValue = JOptionPane.showInputDialog(null,
                "please choose graphics device", "select",
                JOptionPane.INFORMATION_MESSAGE, null,
                items, items[0]);
        GraphicsDevice selected;
        if (selectedValue == null) {
            selected = gs[0];
        } else {
            selected = ((Item) selectedValue).dev;
        }

        WinRobot bot;
        try {
            bot = new WinRobot(selected);
        } catch (AWTException e) {
            JOptionPane.showMessageDialog(null, e.getMessage());
            return;
        }
        System.out.println(bot.toString());

        bot.createScreenCapture(new Rectangle(-65535, -65535, 65535 * 2, 65535 * 2)); //

        JFrame frame = new JFrame();
        if (true) {
            float count = 20;

            if (JOptionPane.showConfirmDialog(null, "Attempt to test the speed of java.awt.Robot.createScreenCapture\r\n\r\n"
                    + "It may take a few minutes",
                    "continue to test?",
                    JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
                Robot robot = new Robot(selected);

                robot.createScreenCapture(
                        new Rectangle(
                        0,
                        0,
                        selected.getDisplayMode().getWidth(),
                        selected.getDisplayMode().getHeight()));
                long startTime = System.currentTimeMillis();
                for (int i = 0; i < count; i++) {
                    if (robot.createScreenCapture(new Rectangle(0, 0, 1280, 800)) == null) {
                        JOptionPane.showMessageDialog(null, "Robot.createScreenCapture failed\r\n");
                        break;
                    }
                    System.gc();
                }
                JOptionPane.showMessageDialog(null, "speed of java.awt.Robot.createScreenCapture\r\n"
                        + "\r\n"
                        + (count * 1000 / ((System.currentTimeMillis() - startTime)))
                        + " frams per second");
            }

            if (JOptionPane.showConfirmDialog(null,
                    "Attempt to test the speed of com.caoym.WinRobot.createScreenCapture\r\n\r\n"
                    + "It may take a few minutes",
                    "continue to test?",
                    JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {

                bot.createScreenCapture(new Rectangle(-65535, -65535, 65535 * 2, 65535 * 2)); //
                long startTime = System.currentTimeMillis();
                for (int i = 0; i < count; i++) {
                    if (bot.createScreenCapture(new Rectangle(-65535, -65535, 65535 * 2, 65535 * 2)) == null) {
                        JOptionPane.showMessageDialog(null, "WinRobot.createScreenCapture failed\r\n");
                    }
                    System.gc();
                }
                JOptionPane.showMessageDialog(null, "the speed of com.caoym.WinRobot.createScreenCapture\r\n\r\n"
                        + (count * 1000 / ((System.currentTimeMillis() - startTime)))
                        + " frams per second");
            }

            if (JOptionPane.showConfirmDialog(null,
                    "Attempt to test UAC / WinLogon screen and cursor capture\r\n\r\n"
                    + "If you click OK,after 10 seconds,the images will be saved as \"temp_screen.bmp\" and \"temp_cursor.bmp\"\r\n"
                    + "So click ok,open  UAC or WinLogon,wait 10+ seconds,and go to current path of this app to find the images file",
                    "continue to test?",
                    JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
                bot.delay(10000);

                WinFileMappingBuffer screen = bot.createScreenCaptureAsFileMapping(new Rectangle(-65535, -65535, 65535 * 2, 65535 * 2));
                if (screen != null) {
                    try {
                        FileChannel foChannel;
                        foChannel = new FileOutputStream("temp_screen.bmp").getChannel();
                        foChannel.write(screen.getBuffer());
                        foChannel.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                WinFileMappingBuffer cursor = bot.captureMouseCursorAsFileMapping(null, null,null);
                if (cursor != null) {
                    try {
                        FileChannel foChannel = new FileOutputStream("temp_cursor.bmp").getChannel();
                        foChannel.write(cursor.getBuffer());
                        foChannel.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                }
            }
            System.gc();

            //JFrame frame = new JFrame();
            frame.add(new WinRobotDemo(bot));
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setSize(500, 800);
            //frame.pack();
            frame.setVisible(true);

        }
        if (JOptionPane.showConfirmDialog(null,
                "\"notepad.exe\" will be open ,and some chars will be inut if ok.\r\n\r\n"
                + "Please DO NOT control keyboad or mouse while testing!",
                "continue to test keyboard input?",
                JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
            System.out.println("start delay");
            bot.delay(1000);
            bot.setAutoDelay(100);
            System.out.println("end delay");
            // WIN + R
            System.out.println("WIN + R");
            bot.keyPress(KeyEvent.VK_WINDOWS);
            bot.keyPress(KeyEvent.VK_R);
            bot.keyRelease(KeyEvent.VK_WINDOWS);
            bot.keyRelease(KeyEvent.VK_R);
            bot.delay(1000);
            System.out.println("open notepad.exe");
            bot.keyPress('N');
            bot.keyRelease('N');
            bot.keyPress('O');
            bot.keyRelease('O');
            bot.keyPress('T');
            bot.keyRelease('T');
            bot.keyPress('E');
            bot.keyRelease('E');
            bot.keyPress('P');
            bot.keyRelease('P');
            bot.keyPress('A');
            bot.keyRelease('A');
            bot.keyPress('D');
            bot.keyRelease('D');
            bot.keyPress(KeyEvent.VK_PERIOD);
            bot.keyRelease('D');
            bot.keyPress('E');
            bot.keyRelease('E');
            bot.keyPress('X');
            bot.keyRelease('X');
            bot.keyPress('E');
            bot.keyRelease('E');
            bot.keyPress(KeyEvent.VK_ENTER);
            bot.keyRelease(KeyEvent.VK_ENTER);
            bot.waitForIdle();
            bot.delay(1000);
            System.out.println("input a~z");
            InputChars(bot, "start testing keyPress&keyRelease:\r\n");
            bot.waitForIdle();
            for (int i = 'A'; i <= 'Z'; i++) {
                bot.keyPress(i);
                bot.keyRelease(i);
            }
            bot.waitForIdle();
            bot.setKeyboardLayout("00000813"); // set Dutch KeyboardLayout
            InputChars(bot, "start testing keyType():\r\n"
                    + "`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./\r\n"
                    + "~!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?\r\n"
                    + "test end");
            //InputChars(bot,"1234567890asdf");
            bot.waitForIdle();
            JOptionPane.showMessageDialog(null, "test keyboard end");
        }

        if (JOptionPane.showConfirmDialog(null,
                "auto move mouse curosr if ok.\r\n\r\n"
                + "Please DO NOT control keyboad or mouse while testing!",
                "continue to test mouse input?",
                JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
            bot.mouseMove(frame.getBounds().x + 100, frame.getBounds().y + 20);
            bot.mousePress(InputEvent.BUTTON1_MASK);
            for (int i = 0; i < 100; i++) {
                bot.mouseMove(i * 4, 100);
            }
            for (int i = 0; i < 100; i++) {
                bot.mouseMove(100, i * 4);
            }
            bot.mousePress(InputEvent.BUTTON1_MASK);
            bot.waitForIdle();
            JOptionPane.showMessageDialog(null, "test mouse end");
        }

        if (JOptionPane.showConfirmDialog(null,
                "",
                "continue to test ATL+CTRL+DEL?",
                JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
            bot.sendCtrlAltDel();
        }

    }
}
