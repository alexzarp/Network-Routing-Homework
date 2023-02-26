package queue

import (
	"sync"

	"github.com/namelew/Network-Routing-Homework/message"
)

type Queue struct {
	mutex    sync.Mutex
	elements []message.Message
}
