/**
 * @Algorithm
 * Shunting Yard algorithm created by Edsger W. Dijkstra.
 * @License
 * This calculator uses material from the Wikipedia article
 * Shunting-yard algorithm (https://en.wikipedia.org/wiki/Shunting-yard_algorithm),
 * which is released under the
 * Creative Commons Attribution-Share-Alike License 3.0 (https://creativecommons.org/licenses/by-sa/3.0/)
 */

package main

import (
	"bufio"
	"fmt"
	"math"
	"os"
	"strconv"
	"unicode"
)

const (
	null string = "nil"
	lpa  string = "left parenthesis"
	rpa  string = "right parenthesis"
	op   string = "operation"
	i32  string = "32 bit integer"
	f32  string = "32 bit floating-point"
)

type token struct {
	sdata        string
	idata        int64
	fdata        float64
	precedence   uint
	tokenType    string
	rAssociative bool
	unary        bool
}

func (t token) toString() string {
	var s string = "("
	s += "\"" + t.sdata + "\", " + strconv.FormatInt(t.idata, 10) + ", " + fmt.Sprintf("%f", t.fdata)
	s += ", [" + strconv.FormatUint(uint64(t.precedence), 10) + " : " + string(t.tokenType)
	return s
}

type lexana struct {
	data string
}

func (l lexana) lex(s string) []token {
	var formatTokens []token
	l.data = s

	i := 0
	abort := false

	for i < len(l.data)-1 {
		c := l.data[i]
		var t token

		for c == ' ' || c == '\t' || c == '\r' {
			if l.data[i] != l.data[len(l.data)-1] {
				i++
				c = l.data[i]
			} else {
				abort = true
				break
			}
		}

		if abort {
			break
		}

		if c == '(' {
			t.tokenType = lpa
			t.sdata = "("
			t.precedence = 9
			t.rAssociative = false
		} else if c == ')' {
			t.tokenType = rpa
			t.sdata = ")"
			t.precedence = 0
			t.rAssociative = false
		} else if c == '+' {
			t.tokenType = op
			t.sdata = "+"
			t.precedence = 2
			t.rAssociative = false
		} else if c == '-' {
			ln := len(formatTokens)
			if ln < 1 {
				t.unary = true
				t.precedence = 5
				t.rAssociative = true
				t.sdata = "m"
				t.tokenType = op
			} else if formatTokens[ln-1].tokenType == op {
				t.unary = true
				t.precedence = 5
				t.rAssociative = true
				t.sdata = "m"
				t.tokenType = op
			} else if formatTokens[ln-1].tokenType == lpa {
				t.unary = true
				t.precedence = 5
				t.rAssociative = true
				t.sdata = "m"
				t.tokenType = op
			} else {
				t.tokenType = op
				t.sdata = "-"
				t.precedence = 2
				t.rAssociative = false
			}
		} else if c == '/' {
			t.tokenType = op
			t.sdata = "/"
			t.precedence = 3
			t.rAssociative = false
		} else if c == '*' || c == 'x' || c == 'X' {
			t.tokenType = op
			t.sdata = "*"
			t.precedence = 3
			t.rAssociative = false
		} else if c == '%' {
			t.tokenType = op
			t.sdata = "%"
			t.precedence = 6
			t.rAssociative = false
		} else if c == '^' {
			t.tokenType = op
			t.sdata = "^"
			t.precedence = 4
			t.rAssociative = true
		} else if unicode.IsDigit(rune(c)) {
			var floatStr string
			t.tokenType = i32

			for unicode.IsDigit(rune(c)) || c == '_' || c == '.' {
				if c == '_' {
					if l.data[i] != l.data[len(l.data)-1] {
						i++
						c = l.data[i]
					} else {
						abort = true
					}
					continue
				} else if c == '.' {
					if t.tokenType == f32 {
						fmt.Println("Redefinition of float:", t.toString())
						os.Exit(1)
					} else {
						t.tokenType = f32
						floatStr += string('.')
					}
				} else {
					floatStr += string(c)
				}

				if l.data[i] != l.data[len(l.data)-1] {
					i++
					c = l.data[i]
				} else {
					abort = true
					break
				}
			}

			if t.tokenType == f32 {
				_fdata, err := strconv.ParseFloat(floatStr, 64)
				if err == nil {
					t.fdata = _fdata
				} else {
					fmt.Println("Error: ", err)
					os.Exit(1)
				}
			} else {
				_idata, err := strconv.Atoi(floatStr)
				if err == nil {
					t.idata = int64(_idata)
				} else {
					fmt.Println("Error: ", err)
					os.Exit(1)
				}
			}

			i-- // Let for loop skip last char.
		} else if c == '.' {
			if unicode.IsDigit(rune(l.data[i+1])) {
				t.tokenType = f32
				if l.data[i] != l.data[len(l.data)-1] {
					i++
					c = l.data[i]
				} else {
					abort = true
				}
				floatStr := "."

				for unicode.IsDigit(rune(c)) || c == '_' || c == '.' {
					if c == '_' {
						if l.data[i] != l.data[len(l.data)-1] {
							i++
							c = l.data[i]
						} else {
							abort = true
						}
						continue
					} else if c == '.' {
						fmt.Println("Redefinition of float:", t.toString())
						os.Exit(1)
					} else {
						floatStr += string(c)
					}

					if l.data[i] != l.data[len(l.data)-1] {
						i++
						c = l.data[i]
					} else {
						abort = true
						break
					}
				}

				_fdata, err := strconv.ParseFloat(floatStr, 64)
				if err == nil {
					t.fdata = _fdata
				} else {
					fmt.Println("Error: ", err)
					os.Exit(1)
				}

				i-- // Let for loop skip last char.
			} else {
				fmt.Println("Unexpected: '.'")
				os.Exit(1)
			}
		} else {
			fmt.Println("Unexpected: '", c, "'")
			os.Exit(1)
		}

		formatTokens = append(formatTokens, t)

		if abort {
			break
		}

		i++
	}

	return formatTokens
}

func shuntingYard(tokens []token) []token {
	var queue []token
	var stack []token

	for _, t := range tokens {
		if t.tokenType == i32 || t.tokenType == f32 {
			queue = append(queue, t)
		} else if t.tokenType == op {
			o1 := t

			for len(stack) > 0 {
				o2 := stack[len(stack)-1]

				if (!o1.rAssociative && o1.precedence <= o2.precedence) || (o1.rAssociative && o1.precedence < o2.precedence) {
					stack = stack[:len(stack)-1]
					queue = append(queue, o2)
					continue
				}

				break
			}

			stack = append(stack, o1)
		} else if t.tokenType == lpa {
			stack = append(stack, t)
		} else if t.tokenType == rpa {
			match := false

			for len(stack) > 0 && stack[len(stack)-1].tokenType != lpa {
				queue = append(queue, stack[len(stack)-1])
				stack = stack[:len(stack)-1]
				match = true
			}

			if !match && len(stack) < 1 {
				fmt.Println("Right parenthesis error:", t.toString())
				os.Exit(1)
			}

			if len(stack) > 0 {
				stack = stack[:len(stack)-1]
			}
		}
	}

	for len(stack) > 0 {
		if stack[len(stack)-1].tokenType == lpa {
			fmt.Println("Mismatched parenthesis error")
			os.Exit(1)
		}

		queue = append(queue, stack[len(stack)-1])
		stack = stack[:len(stack)-1]
	}

	return queue
}

func compute(formatted []token) float64 {
	var stack []float64

	for len(formatted) > 0 {
		t := formatted[0]
		switch t.tokenType {
		case i32:
			stack = append(stack, float64(t.idata))

		case f32:
			stack = append(stack, t.fdata)

		case op:
			if t.unary {
				stack[len(stack)-1] = stack[len(stack)-1] * -1
			} else {
				rhs := stack[len(stack)-1]
				stack = stack[:len(stack)-1]
				lhs := stack[len(stack)-1]
				stack = stack[:len(stack)-1]

				switch t.sdata {
				case "^":
					stack = append(stack, math.Pow(lhs, rhs))

				case "*":
					stack = append(stack, lhs*rhs)

				case "/":
					stack = append(stack, lhs/rhs)

				case "+":
					stack = append(stack, lhs+rhs)

				case "-":
					stack = append(stack, lhs-rhs)
				}
			}
		}

		formatted = formatted[1:]
	}

	return stack[len(stack)-1]
}

func main() {
	var exprStr string
	lexer := new(lexana)
	scanner := bufio.NewScanner(os.Stdin)

	for {
		fmt.Print("Enter an mathematical expression ('exit' to stop): ")
		scanner.Scan()
		exprStr = scanner.Text()
		fmt.Println()

		if exprStr == "exit" {
			break
		}

		formattedTokens := lexer.lex(exprStr)

		formatted := shuntingYard(formattedTokens)

		value := compute(formatted)

		fmt.Println("That evaluates out to:\n", value)
		fmt.Println()
	}
}
