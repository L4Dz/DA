# Scientific Conference Review Assignment Tool

## Project Overview
This project was developed for the **Design of Algorithms (L.EIC016)** course. The primary goal is to implement an organizational tool that automates the assignment of scientific paper submissions to a committee of reviewers using **Maximum Flow** algorithms.

The tool ensures that:
* Each paper is reviewed by a minimum number of qualified experts.
* Reviewer workloads are kept within specified limits.
* Topic expertise (Primary and Secondary) is strictly respected during assignment.

---

## Key Features

### 1. Automated Review Assignment
The tool formulates the assignment problem as a network flow and solves it to find valid paper-reviewer pairings. It determines if a feasible assignment exists given the current constraints.

### 2. Expertise Matching
Assignments are filtered based on topic identifiers (1 to K). A paper is only assigned to a reviewer if the reviewer possesses the required primary or secondary expertise for that paper's topic.

### 3. Risk Analysis
The system evaluates the robustness of the review process. It simulates the absence of reviewers to determine if `MinReviewsPerSubmission` can still be maintained by manipulating parameters like `MaxReviewsPerReviewer`.
---

## Algorithm and Network Formulation

To determine the assignments, the problem is modeled as a **Maximum Flow** problem in a directed graph:

1. **Source (S):** Connected to all **Paper Nodes**. The capacity of these edges is set to `MinReviewsPerSubmission`.
2. **Paper Nodes:** Connected to **Reviewer Nodes**. An edge exists only if the reviewer matches the paper's topic. The capacity is 1.
3. **Reviewer Nodes:** Connected to the **Sink (T)**. The capacity is set to `MaxReviewsPerReviewer`.
4. **Flow Calculation:** If the maximum flow equals (Total Papers × `MinReviewsPerSubmission`), then every paper has been adequately assigned.

---

## Input File Structure

The input files use a specific `.csv` format with sections:

* **Submissions Section:** `PaperID, Title, Authors, Email, PrimaryTopic, SecondaryTopic`
* **Reviewers Section:** `ReviewerID, Name, Email, PrimaryTopic, SecondaryTopic`
* **Control Section:** Specific parameters for the execution of the flow algorithm and risk analysis.

---

## Technical Specifications
* **Language:** C++
* **Algorithms:** Edmonds-Karp or Dinic's Algorithm (Maximum Flow)
* **Data Structures:** Adjacency Lists, Residual Graphs
* **Target Environment:** Linux / MacOS (GCC/Clang)

---

## Team Information
* **Group:** grupo x y 
* **Members:** Afonso Morais, Ladielson Renner, viktoriia kutsel
* **Course:** Analysis and Synthesis of Algorithms / Design of Algorithms (DA)
* **Deadline:** March 30, 2026