- Topic: Analyzing Facebook users

Each account will be a vertex (node) and there will be connection b/w accounts that are friend with each other (edge).

User A wants to make friends with user B. But since user B doesn’t know A, A plan is to ask one of his friends, who is also friend with B, to introduce him. The goal here is to find the shortest path from A to B, and this will be included as a new feature by Facebook.

Facebook also wants to sort users by name / the number of friends, and some more functions.

- Main Function:
    + Finding shortest path
    + Sorting accounts by name / the number of friends
    + Recommend friend with the most common friends for each account
    + Get user info by name, list all duplicated results (sort by name then binary search)

- Note:
    + NodeFile before ConnectionFile (when run)
    
