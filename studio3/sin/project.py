import sys

from sin.output import Output

class Project:

    def __init__(self):

        self.outputs = [Output("output1", "umidi0", 1)]
        self.patterns = []
        self.song = []
        self.delay = 0.5 # seconds between ticks

        # commands
        # cmd_str : (method, n_args, help_str)
        self.cmds = {
            "p" :   (self.cmd_pattern_list, 0, "list patterns"),
            "pl" :  (self.cmd_pattern_list, 0, "list patterns"),
            #"pa" :  self.pattern_add
            "q" :   (self.cmd_quit, 0, "quit"),
            "o" :   (self.cmd_output_list, 0, "List outputs"),
            "oa" :   (self.cmd_output_add, 3, "Add output: <name> <dev> <chan>"),
        }

    def main(self):
        # top event loop
        while(True):
            sys.stdout.write("sin> ")
            sys.stdout.flush()

            try:
                inp = raw_input().strip()
            except EOFError:
                self.cmd_quit(None)

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
        print("Pattern List")
        print(80 * "-")

        for p in self.patterns:
            print(p) 

    # ---[ Outputs ]---
    def cmd_output_list(self, args):
        print("Output List")
        print(80 * "-")

        for o in self.outputs:
            print(o) 

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
