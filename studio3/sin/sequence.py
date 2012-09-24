class Sequence:
    def __init__(self, length):
        self.length = length
        self.events = [ None for i in range(length) ]
