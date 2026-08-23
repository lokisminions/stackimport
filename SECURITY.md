# Security Policy

## Supported Versions

`stackimport` is a pre-1.0 library. Security fixes are applied to the current
development branch (`main`) and, where practical, backported to the latest
release tag.

| Version | Supported          |
| ------- | ------------------ |
| main    | :white_check_mark: |
| latest release | :white_check_mark: |

## Reporting a Vulnerability

Please report suspected vulnerabilities privately via GitHub's security
advisory workflow:

https://github.com/lokisminions/stackimport/security/advisories/new

Please include:

- The stackimport version or commit you are using.
- The input that triggered the issue (a stack file, archive, or ROM sample).
- Steps to reproduce and, if possible, a minimal reproducer.
- Any crash output, sanitizer reports, or logs.

You should receive an acknowledgment within a few business days. Please do not
open a public issue until the report has been triaged and a fix is available.

## Scope

`stackimport` parses untrusted HyperCard stack files, resource forks, and ROM
images, so malformed-input handling is the primary security surface. Reports
about memory-safety issues, parser hangs, resource exhaustion, or unintended
data disclosure are all in scope. Issues in vendored third-party libraries
should still be reported here so they can be forwarded upstream.
