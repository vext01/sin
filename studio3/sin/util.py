import sys, tty, termios
# read unbuffered stdin
# borrowed from:
# http://code.activestate.com/recipes/134892-getch-like-unbuffered-character-reading-from-stdin/
def read_key():
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch
