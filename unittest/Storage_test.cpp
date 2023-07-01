/*
 * Storage_test.cpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

namespace cppknife {
class Person {
public:
  const char *_name;
  int _year;
  int _id;
  static int _nextId;
  Person(const char *name, int year) :
      _name(name), _year(year), _id(_nextId++) {
    printf("created: %d: %s\n", _id, _name);
  }
  Person(const Person &other);
};
int Person::_nextId = 1;

void releasePerson(void *object) {
  Person *person = (Person*) object;
  printf("deleted: %d %s\n", person->_id, person->_name);
  delete person;
}

Person::Person(const Person &other) :
    _name(other._name), _year(other._year), _id(Person::_nextId++) {
  printf("copying person\n");
}
TEST(StorageTest, basic) {
  Storage storage(releasePerson, 1, 1);
  storage.add(new Person("Adam", 22));
  storage.add(new Person("Berta", 35));
  storage.releaseAll();
}

}
/* namespace cppknife */
