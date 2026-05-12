import pandas as pd
import matplotlib.pyplot as plt

# Read CSV
df = pd.read_csv("../combined_simphy.csv")

# Compute the ratio
df["ratio"] = (
    df["nbedges"] /
    (df["nbinternalnodes1"] * df["nbinternalnodes2"])
)


# Compute average ratio for each (pop, n)
grouped = (
    df.groupby(["pop", "n"])["ratio"]
    .mean()
    .reset_index()
)



# All possible n values (sorted)
n_values = sorted(grouped["n"].unique())

# Create plot
plt.figure(figsize=(10, 6))


markers = ['o', 's', '*', 'v']
mindex = 0
# One curve per pop
for pop_value in sorted(grouped["pop"].unique()):

    subset = (
        grouped[grouped["pop"] == pop_value]
        .sort_values("n")
    )

    # Keep only positive finite values
    subset = subset[
        (subset["ratio"] > 0) &
        (subset["ratio"].notna())
    ]

    plt.plot(
        subset["n"],
        subset["ratio"],
        marker=markers[mindex],
        label=f"pop=1e{pop_value}"
    )
    mindex += 1

# Customize x-axis
plt.xticks(n_values, fontsize=13)
plt.tick_params(axis='y', labelsize=14)

# Labels and title
plt.xlabel("Number of leaves", fontsize=16)
plt.ylabel("Average density", fontsize=16)

plt.title(
    "Average density of incompatibility graphs of "
    "SimPhy trees (logscale)", fontsize=18
)


plt.legend(ncol=2,fontsize=14)

plt.grid(True)
plt.yscale("log")

plt.tight_layout()

# Save to PDF
plt.savefig("avg_simphy_density.pdf", format="pdf", bbox_inches="tight")

plt.show()