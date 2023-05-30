# Longest Prefix Matching - C Program

This program was made for a university project for the subject "Data Structures" that I did solo. There was a given routing table in a text file which had some IP subnets that would be read by the program in order to build the tree.

At first, the program reads the routing table text file, translates the decimal digits into binary ones to form the prefixes and create the subtrees based on them. Then, it asks for the user to enter an IP address. When the user inputs an IP address, the program checks if there is any prefix that matches that IP address.

If one prefix is matched, that one is shown, and is the longest prefix matching. If more than one prefixes is matched, it finds the one with the longest prefix. If no prefix is matched, the program notifies the user properly.