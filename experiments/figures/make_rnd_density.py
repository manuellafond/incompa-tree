import pandas as pd
import matplotlib.pyplot as plt

# Read CSV
df = pd.read_csv("../combined_rnd.csv")

# Compute the ratio
df["Average density"] = (
    df["nbedges"] /
    (df["nbinternalnodes1"] * df["nbinternalnodes2"])
)

# Compute average ratio for each nbleaves
grouped = (
    df.groupby("nbleaves")["Average density"]
    .mean()
    .reset_index()
    .sort_values("nbleaves")
)

# X values
x_values = grouped["nbleaves"]

# X-axis labels = nbleaves - 1
x_labels = [str(v - 1) for v in x_values]

# Create plot
plt.figure(figsize=(10, 6))

plt.plot(
    x_values,
    grouped["Average density"],
    marker='o'
)

# Customize x-axis
plt.xticks(x_values, x_labels, fontsize=13)
plt.tick_params(axis='y', labelsize=14)

# Labels and title
plt.xlabel("Number of leaves", fontsize=18)
plt.ylabel("Average density", fontsize=18)

plt.title(
    "Average density of incompatibility graphs of random trees", fontsize=18
)

plt.grid(True)

plt.tight_layout()


# Save to PDF
plt.savefig("average_density_rnd.pdf", format="pdf", bbox_inches="tight")

plt.show()













