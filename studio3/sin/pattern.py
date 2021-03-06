import time
import sys

from sin.sequence import Sequence
from prettytable import PrettyTable
from sin.util import read_key

import os

class Pattern:
    VIEWCONTEXT = 40

    def __init__(self, project, name, length, outputs):

        self.project = project
        self.name = name
        self.length = length    # length in ticks
        self.outputs = outputs  # just a ref to the parent's inst
        self.sequences = [ Sequence() for x in range(len(self.outputs)) ] # index by output

        # sequences indexed by MIDI channel
        self.seqs = [ Sequence() for n in range(len(outputs)) ]

    def __str__(self):
        return "name='%s' length=%s" % (self.name, self.length)

    def play(self, delay):
        for tick in range(self.length):
            fired=False
            for s in range(len(self.sequences)):
                ev = self.sequences[s].get_event_at(tick)

                if ev != "...": # XXX
                    fired = True
                    self.project.key_off(self.outputs[s])
                    self.project.key_on(self.outputs[s], ev)

            fired_s = "*" if fired else " "
            sys.stdout.write("\r%s: %5s/%5s [%s]" % (self.name, tick, self.length-1, fired_s))
            sys.stdout.flush()
            time.sleep(delay)
        print("")


    # Edit a pattern, this could do with curses/urwid, I know XXX
    def edit(self):
        # if no channels XXX
        pos = 0
        ao = 0 # active output

        #os.system("clear")  # yuck
        while True:
            lo = max(0, pos - (Pattern.VIEWCONTEXT/2))
            hi = min(lo + Pattern.VIEWCONTEXT, self.length - 1)

            #print("\33[H")
            #os.system("clear")  # yuck
            print(self.timeline([lo, hi], pos, self.outputs[ao]))

            key = read_key();

            if key == '`':
                break
            elif key == 'j':
                pos = min(pos+1, self.length-1)
            elif key == 'k':
                pos = max(pos-1, 0)
            elif key == 'l':
                ao = min(len(self.outputs)-1, ao+1)
            elif key == 'h':
                ao = max(0, ao-1)
            elif key == 'J':
                new = self.sequences[ao].note_adj_at(False, pos)
                self.project.key_off(self.outputs[ao])
                self.project.key_on(self.outputs[ao], new)
            elif key == 'K':
                new = self.sequences[ao].note_adj_at(True, pos)
                self.project.key_off(self.outputs[ao])
                self.project.key_on(self.outputs[ao], new)
            elif key == ' ':
                for o in self.outputs:
                    self.project.key_off(o)

    def timeline(self, viewport=None, now=-1, active_chan=-1):

        if viewport is None:
            viewport = [0, self.length-1]

        (start, end) = viewport

        t = PrettyTable()
        t.border = 0

        flds = ["Now", "Tick"]
        flds.extend([ "*" + x.name + "*" if x == active_chan else " " + x.name + " " for x in self.outputs ])

        t.field_names = flds

        for i in range(viewport[0], viewport[1]+1):

            marker = ">>>" if now == i else ""

            ev_row = [marker, i]
            ev_row_evs = [ x.get_event_at(i) for x in self.sequences ]
            ev_row.extend(ev_row_evs)

            t.add_row(ev_row)

        return t.get_string()
