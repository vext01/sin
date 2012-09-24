#!/usr/bin/env python

import sys

import sin.project
import sin.pattern
import sin.sequence

version = "0.1"
years = "2012"

def main():

    print("\n" + 80 * '-')
    print("SinStudio v%s" % version)
    print("(c) %s Edd Barrett <vext01@gmail.com>" % years)
    print(80 * '-' + "\n")

    proj = sin.project.Project()
    proj.main()

if __name__ == "__main__":
    main()
