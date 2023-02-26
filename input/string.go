package input

import "strings"

func Sanitaze(s string) string {
	schars := []string{"\n", "\t", "\r", "\a", "\f", "\v", "\b"}

	for _, char := range schars {
		s = strings.ReplaceAll(s, char, "")
	}

	return s
}
