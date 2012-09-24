from sin.sequence import Sequence

class Pattern:
    def __init__(self, length, n_seqs):

        self.length = length    # length in ticks

        # sequences indexed by MIDI channel
        self.seqs = [Sequence(length) for i in range(n_seqs) ]
