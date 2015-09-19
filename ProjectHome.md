The NTRIP protocol distributes streaming GPS data over the internet.

The function of a "caster" is much like an internet radio. It accepts data from a GPS "base station" and broadcasts it over the internet to many "clients".

Typically, each channel (or "mount point") is associated with a single GPS base station. The clients are usually GPS "rovers" which merge the base station's data with their own. With high end equipment, the accuracy can be down to a couple centimeters.

This particular NTRIP "caster" is very lightweight. It is implemented using event driven callbacks rather than threads, and it can be deployed on a micro-controllers like the SLUG (Linksys NSLU2) or the Ubiquity Routerstation.