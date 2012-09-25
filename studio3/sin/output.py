class Output:
    def __init__(self, name, midi_dev, midi_chan):
        self.name = name
        self.midi_dev = midi_dev    # for now, ptr to mio_hdl
        self.midi_chan = midi_chan

    def __str__(self):
        return("name='%s' dev=%s, chan=%s" %
                (self.name, self.midi_dev, self.midi_chan))

