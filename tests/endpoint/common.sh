LEGATO_VERSION="20.04.0"

function download_legato_repo() {
	echo "Downloading Legato AF"
	repo init -u git://github.com/legatoproject/manifest -m legato/releases/${LEGATO_VERSION}.xml
	repo sync
}

function build_legato_repo() {
	if [ ! -d "legato" ]; then
		echo "Please download the Legato AF first"
		exit 1
	fi

	if [ ! -f "legato/Makefile" ]; then
		echo "The Makefile inside legato project is missing"
		echo "Please remove the legato directory and download Legato AF again."
		exit 1
	fi

	echo "Building Legato AF"
	make -C legato localhost
}

function setup_leaf(){
    if ! which "leaf";
    then 
        echo "Please install Legato leaf first"
        exit 1
    fi

    error_message=$(leaf --non-interactive setup "$1" -p "$2" 2>&1)
    if ! echo "$error_message" | grep "Profile name "$1" already exists in current workspace";
    then
        echo "Error occurred when setup leaf profile"
        exit 1
    fi
}
