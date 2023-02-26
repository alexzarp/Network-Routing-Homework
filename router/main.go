package router

import "github.com/namelew/Network-Routing-Homework/queue"

func (r *Router) Route() {

}

func Build(id int) *Router {
	return &Router{
		id:           id,
		gin:          queue.Build(),
		gout:         queue.Build(),
		neighborhood: []neighbor{},
	}
}
