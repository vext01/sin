class Sequence:
    EMPTY = "..."
    START_NOTE = 72 # octave 1 C

    def __init__(self):
        self.events = {}    # maps ticks to events

    def get_event_at(self, tick):
        try:
            ev = self.events[tick]
        except:
            ev = Sequence.EMPTY

        return ev

    def note_adj_at(self, up, x):
        try:
            ev = self.events[x]
        except KeyError:
            self.events[x] = Sequence.START_NOTE
            return

        if up:
            self.events[x] = ev + 1
        else:
            self.events[x] = ev - 1

