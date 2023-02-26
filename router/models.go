package router

import "github.com/namelew/Network-Routing-Homework/queue"

type Router struct {
	id           int
	gin          *queue.Queue
	gout         *queue.Queue
	neighborhood []neighbor
}

type neighbor struct {
	id     int
	cost   int
	parent int
}
