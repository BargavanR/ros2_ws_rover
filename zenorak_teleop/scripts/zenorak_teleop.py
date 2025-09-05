#!/usr/bin/env python3
import sys, termios, tty, threading
import rclpy
from rclpy.node import Node
from std_msgs.msg import String

class TeleopWASD(Node):
    """
    WASD + Enter teleop:
      w->f, a->l, s->b, d->r, Enter->s0
    While a key is active, publishes prefix+0..50 at 10Hz.
    Press Enter to send s0 and reset.
    """
    def __init__(self):
        super().__init__('zenorak_teleop')
        self.pub = self.create_publisher(String, 'zenorak_teleop_cmd', 10)

        self.keymap = {'w':'f', 'a':'l', 's':'b', 'd':'r'}
        self.active_key = None
        self.count = 0
        self.lock = threading.Lock()

        # start keyboard thread (raw mode)
        self._run = True
        self.kthread = threading.Thread(target=self._read_keys, daemon=True)
        self.kthread.start()

        # timer: 10Hz publishing while a key is active
        self.timer = self.create_timer(0.1, self._tick)

    def _read_keys(self):
        fd = sys.stdin.fileno()
        old = termios.tcgetattr(fd)
        tty.setcbreak(fd)            # non-canonical, no Enter needed
        try:
            while self._run:
                c = sys.stdin.read(1)
                with self.lock:
                    if c == '\n':    # Enter => stop
                        self.active_key = None
                        self.count = 0
                        msg = String()
                        msg.data = 's0'
                        self.pub.publish(msg)
                        self.get_logger().info(f'Sent: {msg.data}')
                    elif c in self.keymap:
                        self.active_key = c
                        self.count = 0
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old)

    def _tick(self):
        with self.lock:
            if self.active_key and self.active_key in self.keymap and self.count <= 50:
                msg = String()
                msg.data = f"{self.keymap[self.active_key]}{self.count}"
                self.pub.publish(msg)
                self.get_logger().info(f"Sent: {msg.data}")
                self.count += 1

    def destroy_node(self):
        self._run = False
        super().destroy_node()

def main(args=None):
    rclpy.init(args=args)
    node = TeleopWASD()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
