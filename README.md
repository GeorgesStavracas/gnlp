The GNOME Natural Language Parser
=================================

GNLP is the GNOME Natural Language Parser, a tool that
provides session-wide natural language processing abilities
for GNOME and any other DBus & GLib based applications.

This is achieved by making it a stand-alone server proccess
that clients connect to consume the API.

It's meant to be fast, reliable and extensible. The first is
achieved by avoiding duplicated data (such as strings or any
other input type). The second one, by using state-of-the-art
parsing algorithms. The third point is only made possible by
libpeas plugin library.
