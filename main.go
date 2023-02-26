package main

import (
	"github.com/namelew/Network-Routing-Homework/router"
)

func main() {
	r := router.Build(1)
	r.Route()
}
