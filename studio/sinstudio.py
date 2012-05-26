#!/usr/bin/env python

import urwid
import sys
import os

class SinError(Exception):
    pass

class Channel:
    """ Represents a single channel in the tracker interface """

    def __init__(self, vchan_num, device, dchan_num, parent):
        self.device = device        # serial device node
        self.vchan_num = vchan_num  # virtual channel
        self.dchan_num = dchan_num  # device channel
        self.parent = parent        # parent UI instance

        # make channel widget
        self.title_w = urwid.Text("%d:%s(%d)" %
                (self.dchan_num, self.device, self.vchan_num))
        self.timeline_w = urwid.Edit("", multiline=True) # XXX
        self.pile_w = urwid.Pile([self.title_w, self.timeline_w])

    def get_widget(self):
        return self.pile_w

    def update_timeline(self, keyons):
        text = ""
        for k in keyons:
            if k == None:   # No key fires here
                text += ".....\n"
            else:
                raise SinError("Unknown keyon event")

        self.timeline_w.set_edit_text(text)

class Pattern:

    def __init__(self, pat_no, length, parent_ui):
        self.length = length
        self.parent_ui = parent_ui

        self.pattern_data = {} # chan -> keyons

        # pattern starts empty
        for c in parent_ui.chans:
            self.pattern_data[c.vchan_num] = [ None for x in range(length) ]

class UI:
    """ The main user interface """

    def __init__(self):
        self.version = "0.1"

        self.chans = []
        # hard coded for now XXX - will be able to add multiple sins and pcm
        for i in range(6):
            self.chans.append(Channel(i, "cuaU0", i, self))

        self.title_w = urwid.Text("SinStudio-%s" % self.version)

        self.time_rule_w = urwid.Text("01 \n02 \n03 \n 04 \netc\n", align="right")

        columns = [self.time_rule_w]
        columns.extend([x.get_widget() for x in self.chans])
        self.chan_container_w = urwid.Columns(columns)

        self.toplevel_pile_w = urwid.Pile([self.title_w, self.chan_container_w])
        self.filler_w = urwid.Filler(self.toplevel_pile_w, valign="top")
        self.loop = urwid.MainLoop(self.filler_w)

        # XXX just one pattern for now
        self.patterns = [ Pattern(0, 16, self) ]
        self.cur_pattern = self.patterns[0]
        self.update()

    # XXX needs a scrollable viewport
    def update(self):

        # update channel timelines
        pat = self.cur_pattern

        for c in self.chans:
            keyons = pat.pattern_data[c.vchan_num]
            c.update_timeline(keyons)

    def run(self):
        self.loop.run()

# Main
if __name__ == "__main__":
    u = UI()
    u.run()
