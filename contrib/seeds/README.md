### Seeds ###

Utility to generate the pnSeed[] array that is compiled into the client
(see [src/net.cpp](/src/net.cpp)).

The 600 seeds compiled into the 0.8 release were created from sipa's DNS seed data, like this:

	curl -s http://monetaryunit.sipa.be/seeds.txt | head -1000 | makeseeds.py

The input to makeseeds.py is assumed to be approximately sorted from most-reliable to least-reliable,
with IP:port first on each line (lines that don't match IPv4:port are ignored).
