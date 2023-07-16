# HTTP server in C

This project is an HTTP server implemented in the C programming language.
The server utilizes file-based routing as the default routing mechanism but also
provides support for defining custom routes.

## Disclaimer

This project is primarily developed and tested on my specific configuration, and
therefore, there is no guarantee that it will work flawlessly on other systems or
configurations. As this project was not intended for wide compatibility, it may
require additional modifications or adjustments to function correctly in different
environments. Users are encouraged to adapt the project to suit their specific
setups and consider it as a starting point for their own implementation rather
than a fully production-ready solution.

## Project Description

The server is designed to handle incoming HTTP requests and serve corresponding
responses. It employs a file-based routing system, meaning that by default,
it maps incoming requests to specific files on the server. For example, a
request for '/home' would serve the '/home/index.html' file. This approach
simplifies serving static content such as HTML, CSS, and JavaScript files.

In addition to file-based routing, the server offers the flexibility to define
custom routes. This allows developers to create dynamic routes that execute
custom logic or interact with databases or external APIs. Custom routes can be
used to implement features such as user authentication, data retrieval, or any
other server-side functionality required by the application.

## Getting Started

Make sure you have _gcc_ and _make_ installed.

- download or clone the repository
- run `make` or `make debug` to build the code
- run `./build/main` or `./build/main port_number` to start the server
- (optinal) add you own websites
- visit `localhost:8080` or `localhost:port_number`

## Reflection

Through this project, I aimed to gain an understanding of the mechanics involved
in the functioning of an HTTP server and the underlying HTTP protocol, building
a fully functional HTTP server in the C programming language. Moreover, seamless
communication between the server and web browsers formed an integral part of this
endeavor, ensuring the server's ability to interact with and respond to requests
from modern browsers.

Initially, I set out to build a templating engine with the goal of simplifying
the process of generating dynamic content. However, as I delved deeper into the
feature, I realized that the complexity involved was far greater than I had anticipated.
Parsing, evaluating expressions, handling variable substitutions,
and maintaining template context proved to be significant challenges. Given the
complexity and scope of the task, I decided to table the idea for a future project
when I could dedicate more time and resources to develop a proper templating engine.

During the development process, I encountered challenges with string parsing, particularly
when dealing with parsing specific formats. Manipulating strings in a lower-level
language like C required meticulous attention to detail, manual memory management,
and careful handling of indexes and pointers. It was an error-prone task that demanded
extensive debugging and testing.

Through this experience, I gained a deeper appreciation for modern programming languages
that offer built-in string manipulation libraries and higher-level abstractions,
which significantly simplify the process and reduce human errors.
