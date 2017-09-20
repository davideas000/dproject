#strip test statements

#strip mutiple blank lines, leaving only one
: blank
/^$/{
	N
	/^\n$/D	
}

/testb/,/teste/{
    d
	b blank
}
