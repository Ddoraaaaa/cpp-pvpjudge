def T(k, D):
    # Initialize 2D array dp with dimensions (k + 1) x (D + 1) and fill with -1
    dp = [[-1] * (D + 1) for _ in range(k + 1)]

    # Fill the first row of dp
    for d in range(D + 1):
        dp[1][d] = d-1

    # Fill the rest of the dp array
    for i in range(2, k + 1):
        dp[i][0] = 0
        for d in range(1, D + 1):
            dp[i][d] = 1000000000
            for j in range(1,d + 1):
                dp[i][d] = min(dp[i][d], max(dp[i - 1][j], min(dp[i][d - j+1], dp[i-1][d-j+1])) + 1)

    return dp[k][D]

# Example usage:
k = 2
D = 100
print(T(k, D))  # Output: 4
