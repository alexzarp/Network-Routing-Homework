package queue

import (
	"github.com/namelew/Network-Routing-Homework/message"
)

func Build() *Queue {
	return &Queue{
		elements: make([]message.Message, 0, SIZE),
	}
}

func (q *Queue) Enqueue(m message.Message) {
	q.mutex.Lock()
	defer q.mutex.Unlock()

	if len(q.elements) >= SIZE {
		return
	}

	if len(q.elements) < 1 {
		q.elements = append(q.elements, m)
		return
	}

	q.elements = append(q.elements, m)
}

func (q *Queue) Dequeue() *message.Message {
	q.mutex.Lock()
	defer q.mutex.Unlock()

	if len(q.elements) < 1 {
		return nil
	}

	m := q.elements[HEAD]

	q.elements = q.elements[HEAD+1:]

	return &m
}
