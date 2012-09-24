import sys
import tty, termios

from sin.output import Output
from sin.pattern import Pattern

class Project:

    def __init__(self):

        self.outputs = [
                Output("output1", "umidi0", 1),
                Output("output2", "umidi0", 2)
                ]
        self.patterns = [
                Pattern("pat1", 32, self.outputs)
                ]
        self.song = []
        self.delay = 0.5 # seconds between ticks

        # commands
        # cmd_str : (method, n_args, help_str)
        self.cmds = {
            "p" :   (self.cmd_pattern_list, 0, "list patterns"),
            "pe" :  (self.cmd_pattern_edit, 1, "edit pattern: <patno>"),
            "pl" :  (self.cmd_pattern_list, 0, "list patterns"),
            "pt" :  (self.cmd_pattern_timeline, 1, "show pattern timeline: <patno>"),
            #"pa" :  self.pattern_add
            "q" :   (self.cmd_quit, 0, "quit"),
            "o" :   (self.cmd_output_list, 0, "List outputs"),
            "oa" :   (self.cmd_output_add, 3, "Add output: <name> <dev> <chan>"),
        }

    def main(self):
        # top event loop
        while(True):
            try:
                inp = raw_input("sin> ").strip()
            except EOFError:
                self.cmd_quit(None)

            if len(inp) == 0: continue

            elems = inp.split()
            cmd = elems[0]
            args = elems[1:]

            # lookup command
            for (k, v) in self.cmds.items():
                if k == cmd:
                    if len(args) == v[1]:
                        v[0](args)
                    else:
                        print("Wrong args")
                    break
            else:
                print("Unknown command: %s" % cmd)

    # ---[ Patterns ]---
    def cmd_pattern_list(self, args):
        ct = 0
        for p in self.patterns:
            sys.stdout.write("%s: " % ct)
            print(p) 

    def cmd_pattern_timeline(self, args):
        try:
            pat = self.patterns[int(args[0], 0)]
        except:
            print("Unknwon pattern number")
            return

        print(pat.timeline())

    def cmd_pattern_edit(self, args):
        try:
            pat = self.patterns[int(args[0], 0)]
        except:
            print("Unknwon pattern number")
            return

        pat.edit()

    # ---[ Outputs ]---
    def cmd_output_list(self, args):
        ct = 0
        for o in self.outputs:
            sys.stdout.write("%s: " % ct)
            print(o) 
            ct += 1

    def cmd_output_add(self, args):
        (name, dev, chan) = args

        dev = dev.replace("/dev/", "")
        try:
            chan = int(chan, 0)
        except ValueError:
            print("Bad channel number: %s" % chan)
            return

        out = Output(name, dev, chan)
        self.outputs.append(out)

    # ---[ Misc ]---
    def cmd_quit(self, args):
        print("bye now")
        sys.exit(0)
