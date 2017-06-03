server <- function() {
    writeLines("Listening")
    con <- socketConnection(host="localhost", port = 1771, blocking=TRUE, server=TRUE, open="r+")
    testArray <- c(4, 0, 0, 0, 0)
    writeLines("waiting for pick up")
    writeLines(toString(testArray), con)
    close(con)
}