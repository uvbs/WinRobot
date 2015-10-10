/**
 * 
 */
package com.caoym;

import static org.junit.Assert.*;

import java.awt.AWTException;
import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.image.BufferedImage;
import java.util.Arrays;
import java.util.Collection;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

/**
 * WinRobot unit test
 */
@RunWith(Parameterized.class)
public class WinRobotTest {

    public static class ImageView extends JPanel {

        static final long serialVersionUID = 1L;
        private BufferedImage image;

        public ImageView(BufferedImage image) {
            this.image = image;
        }

        @Override
        public void paint(Graphics g) {

            super.paint(g);
            Graphics2D g2d = (Graphics2D) g;
            g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0f));
            g2d.drawImage(image, 0, 0, image.getWidth(), image.getHeight(), new Color(255, 255, 255, 255), this);

        }

        static JDialog show(BufferedImage image) {

            JDialog dialog = new JDialog();
            dialog.add(new ImageView(image));
            dialog.setSize(image.getWidth() + 100, image.getHeight() + 100);
            dialog.setVisible(true);
            return dialog;
        }
    }

    public static class ColorView extends JPanel {

        static final long serialVersionUID = 1L;
        private Color color;

        public ColorView(Color color) {
            this.color = color;
        }

        @Override
        public void paint(Graphics g) {

            super.paint(g);
            g.setColor(color);
            g.fillRect(0, 0, 100, 100);
        }

        static JDialog show(Color color) {

            JDialog dialog = new JDialog();

            dialog.setSize(100, 100);

            dialog.add(new ColorView(color));

            dialog.setVisible(true);
            return dialog;
        }
    }
    private WinRobot winrobot;
    private GraphicsDevice device;

    @Parameters
    public static Collection<Object[]> winRobots() {


        GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
        GraphicsDevice[] gs = ge.getScreenDevices();

        Object[][] objects = new Object[gs.length][2];
        for (int i = 0; i < gs.length; i++) {
            objects[i][0] = gs[i];
            try {
                objects[i][1] = new WinRobot(gs[i]);
            } catch (AWTException e) {
                e.printStackTrace();
            }
        }
        return Arrays.asList(objects);


    }

    public WinRobotTest(GraphicsDevice device, WinRobot winrobot) {
        this.winrobot = winrobot;
        this.device = device;
    }

    /**
     * Test method for {@link com::caoym::WinRobot::WinRobot()}.
     * Test if dll was loaded and installed
     */
    @Test
    public void testWinRobot() {

        assertNotNull(winrobot);

    }

    /**
     * Test method for {@link com::caoym::WinRobot::createScreenCapture()}.\n
     * 1.test null parameter	->	null BufferedImage\n
     * 2.test empty rectangle	->	null BufferedImage\n
     * 3.test invalid rectangle(new Rectangle(-1,-2,-1,-2))	-> 	null BufferedImage\n
     * 4.test smaller rectangle(new Rectangle(0,0,1,2))	-> 	not empty BufferedImage\n
     * 4.test larger rectangle(new Rectangle(-0x8000000/2,-0x8000000/2,0x7FFFFFF,0x7FFFFFF))	->	full screen BufferedImage\n
     * 5.test full screen		->	full screen BufferedImage\n
     */
    @Test
    public void testCreateScreenCapture() {

        assertNull(winrobot.createScreenCapture(null));

        BufferedImage image = winrobot.createScreenCapture(new Rectangle(0, 0, 0, 0));
        assertNull(image);

        image = winrobot.createScreenCapture(new Rectangle(-1, -2, -1, -2));
        assertNull(image);

        image = winrobot.createScreenCapture(new Rectangle(0, 0, 1, 2));
        assertNotNull(image);
        assertEquals(image.getWidth(), 1);
        assertEquals(image.getHeight(), 2);

        image = winrobot.createScreenCapture(new Rectangle(-0x8000000 / 2, -0x8000000 / 2, 0x7FFFFFF, 0x7FFFFFF));
        assertNotNull(image);

        assertEquals(image.getWidth(), device.getDisplayMode().getWidth());
        assertEquals(image.getHeight(), device.getDisplayMode().getHeight());
        JOptionPane.showMessageDialog(null, "Attempted to call createScreenCapture after 3 seconds.");
        winrobot.delay(3000);
        image = winrobot.createScreenCapture(new Rectangle(0, 0, device.getDisplayMode().getWidth(), device.getDisplayMode().getHeight()));
        assertNotNull(image);
        assertEquals(image.getWidth(), device.getDisplayMode().getWidth());
        assertEquals(image.getHeight(), device.getDisplayMode().getHeight());
        JDialog dialog = ImageView.show(image);
        int op = JOptionPane.showConfirmDialog(dialog,
                "Do you see a image in the test window which is the same as screen ",
                device.getIDstring() + " manually test",
                JOptionPane.YES_NO_OPTION);
        assertEquals(op, JOptionPane.YES_OPTION);
        dialog.setVisible(false);
    }

    /**
     * Test method for speed of {@link com::caoym::WinRobot::createScreenCapture()}.\n
     * call 20 times of CreateScreenCapture with full screen rectangle.failed if speed <= 20fps\n
     */
    @Test
    public void testCreateScreenCaptureSpeed() {
        long current = System.currentTimeMillis();
        for (int i = 0; i < 20; i++) {
            assertNotNull(
                    winrobot.createScreenCapture(
                    new Rectangle(
                    0,
                    0,
                    device.getDisplayMode().getWidth(),
                    device.getDisplayMode().getHeight())));
        }
        assertTrue("createScreenCapture 20 times cost " + (System.currentTimeMillis() - current) + "ms",
                System.currentTimeMillis() <= current + 1000);

    }

    /**
     * Test method for {@link com::caoym::WinRobot::delay(int)}.\n
     * 1. test Delay(0),actual delay time should && <= 100\n
     * 2. test Delay(1000) ,actual delay time should >= 1000 && <= 2000\n
     */
    @Test
    public void testDelay() {
        long current = System.currentTimeMillis();
        winrobot.delay(0);
        assertTrue("Delay(0) actual delay" + (System.currentTimeMillis() - current) + "ms",
                System.currentTimeMillis() <= current + 100);
        current = System.currentTimeMillis();
        winrobot.delay(1000);
        assertTrue("Delay(1000) actual delay" + (System.currentTimeMillis() - current) + "ms",
                (System.currentTimeMillis() >= current + 1000) && (System.currentTimeMillis() <= current + 2000));

    }

    /**
     * Test method for AutoDelay.\n
     * 1.test setAutoDelay and getAutoDelay\n
     * 2.setAutoDelay(1000) and call mouseMove,waitForIdle,actual delay time should >= 1000 && <= 2000. 
     */
    @Test
    public void testAutoDelay() {
        winrobot.setAutoDelay(0);
        assertEquals(winrobot.getAutoDelay(), 0);
        winrobot.setAutoDelay(1000);
        assertEquals(winrobot.getAutoDelay(), 1000);

        long current = System.currentTimeMillis();
        winrobot.mouseMove(0, 0);
        winrobot.waitForIdle();
        assertTrue("setAutoDelay(1000) and call mouseMove,waitForIdle,actual delay " + (System.currentTimeMillis() - current) + "ms",
                (System.currentTimeMillis() >= current + 1000) && (System.currentTimeMillis() <= current + 2000));

        winrobot.setAutoDelay(0);
        assertEquals(winrobot.getAutoDelay(), 0);
    }

    /**
     * Test method for {@link com::caoym::WinRobot::getPixelColor()}.\n
     * Pixel color form screen (createScreenCapture) should be as the same as color form getPixelColor.
     */
    @Test
    public void testGetPixelColor() {

        Color color_0_0 = winrobot.getPixelColor(0, 0);
        Color color_1_2 = winrobot.getPixelColor(1, 2);
        BufferedImage image = winrobot.createScreenCapture(
                new Rectangle(
                0,
                0,
                device.getDisplayMode().getWidth(),
                device.getDisplayMode().getHeight()));

        assertEquals(
                new Color(image.getRGB(0, 0)),
                color_0_0);
        assertEquals(
                new Color(image.getRGB(1, 2)),
                color_1_2);
    }

    /**
     * Test method for {@link com::caoym::WinRobot::isAutoWaitForIdle()}.\n
     * input chars ,set AutoWaitForIdle and AutoDelay,call or not call waitForIdle,then compute time.
     * 1.input 10 chars,AutoWaitForIdle == false, AutoDelay == 0,	waitForIdle  	->	 actual delay time ~= 0\n
     * 2.input 10 chars,AutoWaitForIdle == false, AutoDelay == 1000,no waitForIdle  ->	 actual delay time ~= 0\n
     * 3.input 10 chars,AutoWaitForIdle == false, AutoDelay == 1000,waitForIdle  	->	 actual delay time ~= 10000\n
     * 4.input 10 chars,AutoWaitForIdle == true, AutoDelay == 1000 ,no waitForIdle 	->	 actual delay time ~= 10000\n
     */
    @Test
    public void testAutoWaitForIdle() {

        JDialog dialog = new JDialog();
        JTextField text = new JTextField();
        dialog.setSize(400, 300);
        dialog.add(text);
        dialog.setAlwaysOnTop(true);
        dialog.setVisible(true);


        winrobot.setAutoWaitForIdle(false);
        assertEquals(winrobot.isAutoWaitForIdle(), false);
        winrobot.setAutoDelay(0);
        //1
        long current = System.currentTimeMillis();
        InputChars(winrobot, "1234567890");
        winrobot.waitForIdle();
        assertTrue("actual delay" + (System.currentTimeMillis() - current) + "ms",
                System.currentTimeMillis() <= current + 1000);
        // 2
        winrobot.setAutoDelay(1000);
        current = System.currentTimeMillis();
        InputChars(winrobot, "1234567890");
        assertTrue("actual delay" + (System.currentTimeMillis() - current) + "ms",
                System.currentTimeMillis() <= current + 1000);
        // 3
        winrobot.waitForIdle();
        assertTrue("actual delay" + (System.currentTimeMillis() - current) + "ms",
                (System.currentTimeMillis() >= current + 10000)
                && (System.currentTimeMillis() <= current + 11000));

        // 4
        current = System.currentTimeMillis();
        winrobot.setAutoWaitForIdle(true);
        assertEquals(winrobot.isAutoWaitForIdle(), true);
        InputChars(winrobot, "1234567890");

        dialog.setVisible(false);
        assertTrue("actual delay" + (System.currentTimeMillis() - current) + "ms",
                (System.currentTimeMillis() >= current + 10000) && (System.currentTimeMillis() <= current + 11000));

    }

    /**
     * Test method for {@link com::caoym::WinRobot::keyPress()} \n
     * and {@link com::caoym::WinRobot::keyRelease()}.\n
     * Input a~z,A~Z (use SHIFT key) in a test window,then get the texts from\n
     * the test window,and they should be equal.\n
     * 
     */
    @Test
    public void testKeyPress() {

        JDialog dialog = new JDialog();
        JTextField text = new JTextField();
        dialog.setSize(400, 300);
        dialog.add(text);
        dialog.setAlwaysOnTop(true);
        dialog.setVisible(true);

        winrobot.setAutoDelay(30);
        winrobot.setAutoWaitForIdle(true);

        for (int i = 'A'; i <= 'Z'; i++) {
            winrobot.keyPress(i);
            winrobot.keyRelease(i);
        }
        winrobot.keyPress(KeyEvent.VK_ENTER);
        winrobot.keyRelease(KeyEvent.VK_ENTER);

        winrobot.keyPress(KeyEvent.VK_SHIFT);
        for (int i = 'A'; i <= 'Z'; i++) {
            winrobot.keyPress(i);
            winrobot.keyRelease(i);
        }
        winrobot.keyRelease(KeyEvent.VK_SHIFT);

        dialog.setVisible(false);
        assertEquals(text.getText(), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");


    }

    /**
     * Test method for {@link com::caoym::WinRobot::mouseMove()}.\n
     * 1.mouse move to (0,0),then call CaptureMouseCursor to get current cursor position\n
     * 2.mouse move to (10,20),then call CaptureMouseCursor to get current cursor position\n
     *   in Windows mouse cursor position (x,y) contain normalized absolute coordinates \n
     *   between 0 and 65,535,so may there is 1 pixel aberration
     */
    @Test
    public void testMouseMove() {

        winrobot.setAutoDelay(0);
        winrobot.setAutoWaitForIdle(true);
        Point origin = new Point();
        Point hotspot = new Point();
        winrobot.mouseMove(0, 0);
        int type[] = {WinRobot.CURSOR_UNKNOWN};
        winrobot.captureMouseCursor(origin, hotspot,type);
        assertEquals(origin, new Point(0, 0));

        winrobot.mouseMove(10, 20);

        winrobot.captureMouseCursor(origin, hotspot,null);
        // 
        assertTrue("origin.x = " + origin.x + ",not in 9~10",
                origin.x >= 9 && origin.x <= 10);
        assertTrue("origin.y = " + origin.y + ",not in 19~20",
                origin.y >= 19 && origin.y <= 20);
    }

    /**
     * Test method for {@link com::caoym::WinRobot::mousePress()} \n
     * and {@link com::caoym::WinRobot::mouseRelease()}.\n
     * Once the test running,a dialog is opened which have a button and will \n
     * close if it is clicked it,the test move the mouse and simulate user click\n
     * to close the dialog a dialog and use mouseMove/mousePress/mouseRelease to\n
     * close it.
     */
    @Test
    public void testMousePress() {
        JOptionPane.showMessageDialog(
                null, 
                "Once the test running,a dialog is opened which have a button and will \n"
                + "close if it is clicked it,the test move the mouse and simulate user click\n"
                + "to close the dialog a dialog and use mouseMove/mousePress/mouseRelease to\n"
                + "close it");
        winrobot.setAutoDelay(1000);
        winrobot.setAutoWaitForIdle(false);

        winrobot.mouseMove(200, 150);

        winrobot.mousePress(InputEvent.BUTTON1_MASK);
        winrobot.mouseRelease(InputEvent.BUTTON1_MASK);

        final JDialog dialog = new JDialog();
        JButton button = new JButton();
        button.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent arg0) {
                dialog.setVisible(false);

            }
        });
        dialog.setLocation(0, 0);
        dialog.setSize(400, 300);
        dialog.add(button);
        dialog.setModal(true);
        dialog.setAlwaysOnTop(true);
        dialog.setVisible(true);

        int op = JOptionPane.showConfirmDialog(dialog,
                "Was the test dialog closed by WinRobot?",
                device.getIDstring() + " manually test",
                JOptionPane.YES_NO_OPTION);
        assertEquals(op, JOptionPane.YES_OPTION);


    }

    /**
     * Test method for {@link com::caoym::WinRobot::mouseWheel()}.\n
     * Open any window which has scroll bar,make show that window is top and \n
     * enable and have focus.mouseWheel will start after 3 seconds ,and the window\n
     * should be scrolled up and down quickly(up 10 wheel-equipped mice,then down\n
     * 10),then see whether it works.
     */
    @Test
    public void testMouseWheel() {

        JOptionPane.showMessageDialog(null, "Open any window which has scroll bar,\r\n"
                + "make show that window is top and enable and have focus,\r\n"
                + "mouseWheel will start after 3 seconds when you click the \"ok\",\r\n"
                + "and the window should be scrolled up and down quickly(up 10 wheel-equipped mice,then down 10)");

        winrobot.delay(3000);
        winrobot.setAutoDelay(1000);
        winrobot.setAutoWaitForIdle(true);
        winrobot.mouseWheel(10);
        winrobot.mouseWheel(-10);
        
        int op = JOptionPane.showConfirmDialog(null,
                "Is mouseWheel work?",
                device.getIDstring() + " manually test",
                JOptionPane.YES_NO_OPTION);
        assertEquals(op, JOptionPane.YES_OPTION);

    }

    /**
     * Test method for {@link com::caoym::WinRobot::captureMouseCursor()}.\n
     * Show the captured cursor image in a test window,and the red cross center is the "hotspot".
     */
    @Test
    public void testCaptureMouseCursor() {

        JOptionPane.showMessageDialog(null, "captureMouseCursor will start after 3 seconds when you click the \"ok\",\r\n"
                + "and will show the captured cursor image in a pupup window,and the red cross center is the \"hotspot\",then see whether is work.");
        winrobot.delay(3000);
        Point hotspot = new Point();
        Point origin = new Point();

        BufferedImage image = winrobot.captureMouseCursor(origin, hotspot,null);
        assertNotNull(image);
        // draw a red cross which center is the "hotspot"
        for (int x = 0; x < image.getWidth(); x++) {
            image.setRGB(x, hotspot.y, 0xFFFF0000);
        }
        for (int y = 0; y < image.getHeight(); y++) {
            image.setRGB(hotspot.x, y, 0xFFFF0000);
        }

        JDialog dialog = ImageView.show(image);
        int op = JOptionPane.showConfirmDialog(null,
                "Do you see a image in the test window\r\nwhich is the same as current cursor?"
                + "And red cross center is \"hotspot\"?\r\n",
                device.getIDstring() + " manually test",
                JOptionPane.YES_NO_OPTION);

        dialog.setVisible(false);
        assertEquals(op, JOptionPane.YES_OPTION);

    }

    /**
     * Test method for {@link com::caoym::WinRobot::keyType()}.\n
     * Input dutch chars "it is just a selection of unicode characters:§±¨¨|°¼5¢ß€¶µ£测试"\n
     * in a test windows ,then get the texts from the test window,and they should be equal.\n
     */
    @Test
    public void testKeyType() {

        JDialog dialog = new JDialog();
        JTextField text = new JTextField();
        dialog.setSize(400, 300);
        dialog.add(text);
        dialog.setAlwaysOnTop(true);
        dialog.setVisible(true);

        winrobot.setAutoDelay(100);
        winrobot.setAutoWaitForIdle(true);

        InputChars(winrobot, "it is just a selection of unicode characters:§±¨¨|°¼5¢ß€¶µ£测试");
        winrobot.delay(1000);
        dialog.setVisible(false);
        assertEquals(text.getText(), "it is just a selection of unicode characters:§±¨¨|°¼5¢ß€¶µ£测试");

    }

    /**
     * Test method for {@link com::caoym::WinRobot::sendCtrlAltDel()}.\n
     * Just call sendCtrlAltDel,then see if it works.
     */
    @Test
    public void testSendCtrlAltDel() {

        JOptionPane.showMessageDialog(null, "Click ok to test sendCtrlAltDel.");


        winrobot.setAutoDelay(0);
        winrobot.setAutoWaitForIdle(true);

        winrobot.sendCtrlAltDel();

        int op = JOptionPane.showConfirmDialog(null,
                "Is sendCtrlAltDel work?",
                device.getIDstring() + " manually test",
                JOptionPane.YES_NO_OPTION);
        assertEquals(op, JOptionPane.YES_OPTION);
    }

    public static void InputChars(WinRobot bot, String str) {
        for (int i = 0; i < str.length(); i++) {
            bot.keyType(str.charAt(i));
        }

    }
}
