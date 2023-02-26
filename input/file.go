package input

import (
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"
)

type router struct {
	rid    string
	adress string
	port   string
}

func LoadArgs(rid int) {
	dr, err := os.ReadFile("./data/routers.config")
	var router router

	router.rid = strconv.Itoa(rid)

	if err != nil {
		log.Fatalln("LoadArgs Error: ", err.Error())
	}

	for _, line := range strings.Split(string(dr), "\n") {
		column := strings.Split(Sanitaze(line), " ")
		if len(column) > 0 && column[0] == router.rid {
			router.port = column[1]
			router.adress = column[2]
		}
	}

	fmt.Println(router)
}
