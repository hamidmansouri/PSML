# PSML Project Makefile
tutorial:
	@# todo: have this actually run some kind of tutorial wizard?
	@echo "Please read the 'Makefile' file to go through this tutorial"

var-lost:
	export foo=bar
	echo "foo=[$$foo]"
