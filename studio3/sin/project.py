import sys
import ctypes

from sin.output import Output
from sin.pattern import Pattern

class Project:

    DELAY = 0.1

    MIO_OUT =   4
    MIO_IN =    8

    def __init__(self):

        # We call to C for mio_* calls
        self.libsndio = ctypes.cdll.LoadLibrary("libsndio.so")

        # mio_open(3)
        self.libsndio.mio_open.argtypes = [ctypes.c_char_p, ctypes.c_uint, ctypes.c_int]
        self.libsndio.mio_open.restype = ctypes.c_void_p

        # mio_close(3)
        self.libsndio.mio_close.argtypes = [ctypes.c_void_p]

        # mio_write(3)
        self.libsndio.mio_write.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_ulonglong]
        self.libsndio.mio_write.restype = ctypes.c_ulonglong

        # Needs to be a dict and the ability to add/remove
        self.mio_hdl = self.mio_open("rmidi/0", Project.MIO_OUT, 0)
        if self.mio_hdl == None:
            print("Failed to open midi IO")
            sys.exit(1)

        # for now this is all test data XXX
        self.outputs = [
                Output("output1", self.mio_hdl, 1),
                Output("output2", self.mio_hdl, 2),
                Output("output3", self.mio_hdl, 3),
                Output("output4", self.mio_hdl, 4),
                Output("output5", self.mio_hdl, 5),
                Output("output6", self.mio_hdl, 6),
                ]
        # for now this is all test data XXX
        self.patterns = [
                Pattern(self, "pat1", 32, self.outputs)
                ]
        self.song = []
        self.delay = 0.5 # seconds between ticks XXX hook in
        self.devs = {}   # midi device node ptrs (native)

        # commands
        # cmd_str : (method, n_args, help_str)
        self.cmds = {
            "p" :   (self.cmd_pattern_list, 0, "list patterns"),
            "pe" :  (self.cmd_pattern_edit, 1, "edit pattern: <patno>"),
            "pl" :  (self.cmd_pattern_list, 0, "list patterns"),
            "pp" :  (self.cmd_pattern_play, 1, "play pattern: <patno>"),
            "pt" :  (self.cmd_pattern_timeline, 1, "show pattern timeline: <patno>"),
            #"pa" :  self.pattern_add
            "q" :   (self.cmd_quit, 0, "quit"),
            "o" :   (self.cmd_output_list, 0, "List outputs"),
            "oa" :   (self.cmd_output_add, 3, "Add output: <name> <dev> <chan>"),
            }

    def key_on(self, output, note_val):
        msg = str(bytearray([0x90 + output.midi_chan, note_val & 0x7f, 0x7f]))
        ret = self.mio_write(output.midi_dev, msg, 3)

        if ret != 3:
            print("MIDI message under-run")

    def key_off(self, output):
        msg = str(bytearray([0x80 + output.midi_chan, 0, 0x7f]))
        ret = self.mio_write(output.midi_dev, msg, 3)

        if ret != 3:
            print("MIDI message under-run")

    def mio_open(self, name, bio, flag):
        return self.libsndio.mio_open(name, bio, flag)

    def mio_close(self, mio_hdl_ptr):
        self.libsndio.mio_close(mio_hdl_ptr) # returns void

    def mio_write(self, mio_hdl_ptr, midi_bytes_ptr, nbytes):
        return self.libsndio.mio_write(mio_hdl_ptr, midi_bytes_ptr, nbytes)

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

    def cmd_pattern_play(self, args):
        try:
            pat = self.patterns[int(args[0], 0)]
        except:
            print("Unknwon pattern number")
            return

        pat.play(Project.DELAY)

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
