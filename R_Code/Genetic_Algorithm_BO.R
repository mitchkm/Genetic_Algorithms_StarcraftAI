# Helper Functions and Variables: 
result_Headers <- "Win, Total_Minerals, Total_Gas, Custom_score, Unit_Score, Kill_Score, Building_Score, Razing_Score, Elapsed_Time"
indvInfo <- c(1,2,3,4)

report <- function(...) {
  writeLines(paste(..., sep = ""))
}

createID <- function(gen, indv, game) {
  id <- paste("Gen", str_pad(gen, 5, side = "left", pad = "0"),
              "_Indv", str_pad(indv, 4, side = "left", pad = "0"),
              "_Game", str_pad(game, 2, side = "left", pad = "0"),
              sep = "")
  returnValue(id)
}

# Simulates a single game of starcraft and returns the resulting scores of that game
#   Example Data: 
#     >      Win Total_Minerals Total_Gas Custom_score Unit_Score Kill_Score Building_Score Razing_Score Elapsed_Time
#     >  (1)   0            922         0            0        350          0            450            0          465
#
simGame <- function(ID, BO_Indv) {
    # connect to starcraft AI 
    report(ID, ": Establishing connection to AI...")
    con <- socketConnection(host="localhost", port = 1771, blocking=TRUE, server=TRUE, open="r+")
    
    #Send Build Order 
    writeLines(BO_Indv, con)
    
    report(ID, ": Recieved build order...")
    msg <- readLines(con, 1, skipNul = T)
    report(ID, "_AI: ", msg)
    
    # Send Unique ID
    writeLines(ID, con)
    
    # get results & convert to table
    report(ID, ": Waiting for end of game...")
    result <- readLines(con, 1, skipNul = T)
    
    txtCon <- textConnection(paste(result_Headers,"\n", result))
    scores <- read.csv(txtCon, header = T)
    close(txtCon)
    
    report(ID, "_AI: \n")
    show(scores)
    
    # Send msg to AI to restart game
    writeLines("continue", con)
    
    close(con)
    
    returnValue(scores)
}

### Config ###
MAX_GEN <- 50
POP_SIZE <- 10
GAMES_PER_INDV <- 5

# etc.

##############

main <- function() {
  gen <<- 1
  
  # instatiate matrix containing all build orders
  # each entry in 'builds' contains a list with the first element containg the build order size
  #   Example: generation: 1 individual: 1
  #     > matrix[[1,1]]
  #     > [1] 5 0 0 0 0 0
  #   This build order would build 5 drones/workers/probes
  builds <<- matrix(list(), MAX_GEN, POP_SIZE)
  
  # Instatiate data frame containing every gen, indv, game, fitness, and game results
  run_data.row <<- 1
  run_data <<- data.frame(Gen=1, Indv=1, Game=1, Fitness=NA,
                         Win=NA,
                         Total_Minerals=NA,
                         Total_Gas=NA,
                         Custom_Score=NA,
                         Unit_Score=NA,
                         Kill_Score=NA,
                         Building_Score=NA,
                         RAzing_Score=NA,
                         Elapsed_Time=NA)
  
  return()
  temporary <- c(5, 0, 0, 0, 2, 3)
  
  # TODO: generate first generation individuals
  
  # Full run of the Genetic Algorithm
  while(gen <= MAX_GEN) {
    indv <<- 1
    while(indv <= POP_SIZE) {
      game <<- 1
      while(game <= GAMES_PER_INDV){
        
        run_data[run_data.row, +indvInfo] <<- c(gen, indv, game, NA)
        
        run_data[run_data.row, -indvInfo] <<- simGame(ID=createID(gen, indv, game),
                                                      BO_Indv=toString(temporary))
        
        run_data.row <<- run_data.row + 1
        game <<- game + 1
      }
      indv <<- indv + 1
    }
    
    # TODO: calc. fitness, generate next generation
    
    gen <<- gen + 1
  }
}



test <- function() {
  example_BO <- c(4,0,0,0,2)
  
  while(TRUE)
    simGame(BO_Indv = toString(example_BO), ID = "Gen01_Indv02")
}