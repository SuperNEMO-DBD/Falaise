#!/usr/bin/python
# -*- mode: python; -*-
# mkuniqueflags.py
# Author: F. Mauger <mauger@lpccaen.in2p3.fr>
# Last modified: 2008-04-11
#
# Usage:
#
# shell>$ echo "-a -b -c -c -d -d -e -e -a -f -z" | programs/mkuniqueflags.py
# -a -b -c -d -e -f -z 
#
# Use reverse order:
#
# shell>$ echo -a -b -c -c -d -d -e -e -a -f -z | programs/mkuniqueflags.py -r
# -b -c -d -e -a -f -z 
#

import sys

TheDebug=False
TheTest=False
TheReverse=False

class MkUniqueFlags :
    "Application to build ordered list of unique compilation flags"
    
    def __init__ (self):
        self.__libtags = []
        self.__debug = False
        self.__direction = "normal"
        self.__lib = False
        
    def set_reverse (self) :
        self.__direction = "reverse"

    def set_lib (self) :
        self.__lib = True

    def set_normal (self) :
        self.__direction = "normal"

    def is_reverse (self) :
        return self.__direction == "reverse"
    
    def is_lib (self) :
        return self.__lib
    
    def is_normal (self) :
        return self.__direction == "normal"
    
    def append (self, tag_) :
        tag = tag_.strip ()
        if self.__libtags.count (tag) == 0 :
            self.__libtags.append (tag)
            if self.__debug :
                sys.stderr.write ("%s: debug: Appending tag '%s' in the list!\n"
                                 % (self.__class__.__name__, tag));
        else :
            if self.__debug :
                sys.stderr.write ("%s: debug: Tag '%s' already in the list!\n"
                                 % (self.__class__.__name__, tag));

    def prepend (self, tag_) :
        tag = tag_.strip ()
        if self.__libtags.count (tag) == 0 :
            self.__libtags.insert (0, tag)
            if self.__debug :
                sys.stderr.write ("%s: debug: Prepending tag '%s' in the list!\n"
                                 % (self.__class__.__name__, tag));
        else :
            if self.__debug :
                sys.stderr.write ("%s: debug: Tag '%s' already in the list!\n"
                                 % (self.__class__.__name__, tag));

    def print_tags (self) :
        for tag in self.__libtags:
            sys.stdout.write ("%s " % tag)
        sys.stdout.write ("\n")
        pass

    def set_debug (self , debug_) :
        self.__debug = debug_;
        
    def run (self) :
        line = sys.stdin.readline ()
        if self.__debug :
            sys.stderr.write ("%s: debug: line is '%s'\n"
                             % (self.__class__.__name__, line[0:-1]))
            sys.stderr.write ("%s: debug: direction is '%s'\n"
                             % (self.__class__.__name__, self.__direction))
        if line[0:-1] != "" :
            tokens = line[0:-1].split ()
            if self.is_lib () :
                for token in tokens:
                    if self.__debug :
                        sys.stderr.write ("%s: debug: normal token is '%s'\n"
                                          % (self.__class__.__name__, token))
                    if token.strip () != "" :
                        if len (token) > 2 :
                            if token[0:2] == "-L" :
                                self.append (token.strip ());
                        
                tokens_rev = line[0:-1].split ()
                tokens_rev.reverse ()
                for token in tokens_rev :
                    if self.__debug :
                        sys.stderr.write ("%s: debug: reverse token is '%s'\n"
                                          % (self.__class__.__name__, token))
                    if token.strip () != "" :
                        if len (token) > 2 :
                            if token[0:2] == "-L" :
                                continue
                        self.prepend (token.strip ());
            else:
                if self.is_normal () :
                    for token in tokens:
                        if self.__debug :
                            sys.stderr.write ("%s: debug: normal token is '%s'\n"
                                              % (self.__class__.__name__, token))
                        if token.strip () != "" :
                            self.append (token.strip ());
                else:
                    tokens.reverse ()
                    for token in tokens :
                        if self.__debug :
                            sys.stderr.write ("%s: debug: reverse token is '%s'\n"
                                              % (self.__class__.__name__, token))
                        if token.strip () != "" :
                            self.prepend (token.strip ());                
        self.print_tags ()
        
    def test (self) :
        self.append ("-L/usr/lib")
        self.append ("  -L/usr/local/lib ")
        self.append ("-L/usr/test1/lib")
        self.append (" -L/usr/test2/lib")
        self.append ("-L/usr/test3/lib")
        self.append ("-ltoto1")
        self.append ("-ltoto2")
        self.append ("-ltoto3")
        self.append ("-L/usr/lib")
        self.append (" -L/usr/local/lib")
        self.append ("-L/usr/test2/lib  ")
        self.append ("-L/usr/test4/lib")
        self.append ("-ltoto2")
        self.append ("-lbibi")
        self.print_tags ()
        pass

# Main:
if __name__ == '__main__':
    import getopt
    TheDebug = False
    TheTest = False
    TheReverse = False
    TheLib = False
    try:
        (opts, args) = getopt.getopt (sys.argv[1:], 'dtrl')
    except getopt.error:
        sys.stderr.write ("Usage: %s [-d|-t|-r|-l] \n" % sys.argv[0])
        sys.exit (1)
    for (opt, val) in opts:
        if opt == '-d':
            TheDebug = True
        elif   opt == '-t':
            TheTest = True
        elif   opt == '-r':
            TheReverse = True
        elif   opt == '-l':
            TheLib = True
        else:
            # This should never happen:
            raise OptionError, opt
    for (arg, val) in args:
        if arg == 'zzz':
            pass
        else:
            pass

    theApp = MkUniqueFlags ()
    theApp.set_debug (TheDebug);
    if TheLib :
        theApp.set_lib ()
    elif TheReverse :
        theApp.set_reverse ()
    if TheTest :
        theApp.test ()
    else :
        theApp.run ()
  
# end
