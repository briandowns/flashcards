package main

import "gorm.io/gorm"

// Card
type Card struct {
	gorm.Model
	Front string
	Back  string
}

type User struct {
	gorm.Model
	FirstName string
	LastName  string `json:last_name`
	Email     string
}
