library(GA)
library(stringr)
setwd("C:/Users/mitch/Documents/SCBW_AI/R_Code")

# # Altered GA package functions
# gaperm_Population <- function(object, size=0, r=FALSE,...)
# {
#   # Generate popSize random permutations in the range [min, max]
#   int <- seq.int(object@min, object@max)
#   n <- if (size > 0) size else length(int)
#   population <- matrix(NA, nrow = object@popSize, ncol = n)
#   for(i in 1:object@popSize)
#     population[i,] <- if(size > 0 && r)  sample.int(object@max, size, replace = r) else sample(int, replace=F)
#   return(population)
# }

# Altered GA package function to create a population with varied number of parameters per individual
gaperm_Population_varied <- function(object, size=c(1, 1, 1), r=FALSE,...)
{
  if (length(size) != object@popSize) return(NULL)
  int <- seq.int(object@min, object@max)
  # Generate popSize random permutations with varied lenght in the range [min, max]
  population <- matrix(list(), nrow = object@popSize, ncol = 1)
  for(i in 1:object@popSize)
    population[[i]] <- sample(int, size[i], replace = r)
  object@population <- population
  return(object)
}

# Altered GA package function to create two children by picking a different crossover point in each
#  list of numbers and swapping them
ga_spCrossover_varied <- function(object, parents, ...)
{
  # Single-point crossover
  fitness <- object@fitness[parents]
  parents <- object@population[parents,,drop = FALSE]
  lens    <- lengths(parents[,1])
  children <- matrix(list(), nrow = 2, ncol = 1)
  fitnessChildren <- rep(NA, 2)
  pnt <- c(sample(1:lens[1], size = 1), sample(1:lens[2], size = 1))
  crossMethod <- sample(1:10, size = 1)
  if (crossMethod > 5) {
  children[1,1] <- list(c(parents[[1]][1:pnt[1]],
                          parents[[2]][pnt[2]:lens[2]]))
  children[2,1] <- list(c(parents[[2]][1:pnt[2]],
                          parents[[1]][pnt[1]:lens[1]]))
  }
  else {
    children[1,1] <- list(c(parents[[2]][pnt[2]:lens[2]], 
                            parents[[1]][1:pnt[1]]))
    children[2,1] <- list(c(parents[[1]][pnt[1]:lens[1]],
                            parents[[2]][1:pnt[2]]))
  }
  
  out <- list(children = children, fitness = fitnessChildren)
  return(out)
}


# Helper Functions and Variables: 
result_Headers <- "Win, Total_Minerals, Total_Gas, Custom_score, Unit_Score, Kill_Score, Building_Score, Razing_Score, Elapsed_Time"
indvInfo <- c(1,2,3,4)
VAL_NAMES <- c("SCV", "Marine", "Supply_Depot", "Barracks", "Push_Marines")

report <- function(...) {
  writeLines(paste(..., sep = ""))
}

toBOString <- function(bo) {
  return(paste(length(bo), ", ", toString(bo), sep=""))
}

createID <- function(gen, indv, game) {
  id <- paste("Gen", str_pad(gen, 5, side = "left", pad = "0"),
              "_Indv", str_pad(indv, 4, side = "left", pad = "0"),
              "_Game", str_pad(game, 2, side = "left", pad = "0"),
              sep = "")
  returnValue(id)
}

pad <- function(x, fill = NA) {
  max_length <- max(lengths(x))
  sapply(x, function(y){
    c(y, rep(fill, max_length - length(y)))
  })
  
}

numToBO <- function(y) {
  for(v in 1:length(VAL_NAMES)) {
    y[y==(v-1)] <- VAL_NAMES[v]
  }
  return(y)
}

meaningfulBO <- function(x) {
  sapply(x, numToBO)
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
    
    report(ID, ": Sending build order...")
    msg <- readLines(con, 1, skipNul = T)
    report(ID, "_AI: ", msg)
    
    # Send Unique ID
    writeLines(ID, con)
    
    # get results & convert to table
    report(ID, ": Waiting for end of game(results)...")
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

####### Config #######
MAX_GEN <- 18
POP_SIZE <- 50
GAMES_PER_INDV <- 5

MAX_INSTR <- 4
MIN_ORDERS <- 50
MAX_ORDERS <- 350

# Build GA-package ga class object
ga <- new('ga')
ga@min <- 0
ga@max <- MAX_INSTR
ga@popSize <- POP_SIZE

# GA specific params
SELECT <- ga_tourSelection
TOUR_SIZE <- 5

CROSS <- ga_spCrossover_varied
MAX_ATTEMPTS <- 10

fitFun <- function(data) {
  fitness <- mean((data$Unit_Score + data$Building_Score + data$Kill_Score + data$Razing_Score) 
                  / data$Elapsed_Time)
  return(fitness)
}

# etc.

####################

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
                         Razing_Score=NA,
                         Elapsed_Time=NA)
  
  # Generate first generation individuals
  #ga <<- gaperm_Population_varied(ga, size=sample(c(MIN_ORDERS:MAX_ORDERS), ga@popSize, replace=T), r=T)
  
  # return(ga@popSize)
  
  # Full run of the Genetic Algorithm
  while(gen <= MAX_GEN) {
    indv <<- 1
    builds[gen,] <<- ga@population
    while(indv <= POP_SIZE) {
      game <<- 1
      while(game <= GAMES_PER_INDV){
        
        run_data[run_data.row, +indvInfo] <<- c(gen, indv, game, NA)
        
        run_data[run_data.row, -indvInfo] <<- simGame(ID=createID(gen, indv, game),
                                                      BO_Indv=toBOString(ga@population[[indv]]))
        
        run_data.row <<- run_data.row + 1
        game <<- game + 1
      }
      run_data[run_data$Gen==gen & run_data$Indv==indv,]$Fitness <<-
        fitFun(run_data[run_data$Gen==gen & run_data$Indv==indv,])
      
      indv <<- indv + 1
    }
    # inject fitness into ga class object
    ga@fitness <- run_data[run_data$Gen==gen & run_data$Game==1,]$Fitness
    
    # select parents to use to create next generation
    selection <- SELECT(ga, k = TOUR_SIZE)
    report("====== Selected Parent Indv. for Gen ", gen, " ======")
    print(sort(unique(match(selection$fitness, ga@fitness))))
    
    ga@population <- selection$population
    
    # crossover (Copied from the GA package's ga function and altered)
    
    Pop <- ga@population
    nmating <- floor(ga@popSize/2)
    mating <- matrix(sample(1:(2*nmating), size = (2*nmating)), ncol = 2)
    for(i in seq_len(nmating))
    { 
      parents <- mating[i,]
      attempts <- 0
      repeat {
        attempts <- attempts + 1
        crossover <- CROSS(ga, parents)
        childLens <- lengths(crossover$children[,1])
        if (sum(childLens > MIN_ORDERS & childLens < MAX_ORDERS) == 2) {
          break;
        } 
        else if (sum(childLens > MIN_ORDERS) == 2 & attempts > MAX_ATTEMPTS) {
          if (childLens[1] > MAX_ORDERS) {
            length(crossover$children[[1,1]]) <- MAX_ORDERS
          } 
          if (childLens[2] > MAX_ORDERS) {
            length(crossover$children[[2,1]]) <- MAX_ORDERS
          }
          break;
        }
      }
      
      Pop[parents,] <- crossover$children
    }
    ga@population <- Pop
    
    
    gen <<- gen + 1
  }
}

writeData <- function(name, gen = 1) {
  write.csv(run_data, file = paste("data/", name, "/", name, ".csv", sep = ''))
  for (x in 1:gen) {
    write.csv(t(meaningfulBO(pad(builds[x,]))), file = paste("data/", name, "/", name, "_mbuilds_gen_", x, ".csv", sep = ''))
  }
  save(builds, run_data, file = paste("data/", name, "/", name, ".Rdata", sep = ''))
}

test <- function(iter = 5) {
  i <- 1
  while(i < iter) {
    simGame(BO_Indv = toBOString(builds[[58]]), ID = "!TEST!")
    i <- i + 1
    }
}

continue <- function() {
  while(gen <= MAX_GEN) {
    indv <<- 41
    #builds[gen,] <<- ga@population
    while(indv <= POP_SIZE) {
      game <<- 5
      while(game <= GAMES_PER_INDV){
        
        run_data[run_data.row, +indvInfo] <<- c(gen, indv, game, NA)
        
        run_data[run_data.row, -indvInfo] <<- simGame(ID=createID(gen, indv, game),
                                                      BO_Indv=toBOString(ga@population[[indv]]))
        
        run_data.row <<- run_data.row + 1
        game <<- game + 1
      }
      run_data[run_data$Gen==gen & run_data$Indv==indv,]$Fitness <<-
        fitFun(run_data[run_data$Gen==gen & run_data$Indv==indv,])
      
      indv <<- 51
    }
    gen <<- gen + 1
  }
}