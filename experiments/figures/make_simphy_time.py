import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("../merged_simphy.csv")

# Columns containing timings
time_cols = ["fasttime", "naivetime", "bhvtime"]

# Average repeated measurements for each (pop, n)
avg_df = (
    df.groupby(["pop", "n"])[time_cols]
      .mean()
      .reset_index()
)

# Marker styles (changes with pop)
markers = ["o", "s", "^", "D", "x", "*", "v", "P", "H"]

# Line styles (changes with timing type)
line_styles = {
    "fasttime": "-",
    "naivetime": "--",
    "bhvtime": ":"
}

# Create figure
plt.figure(figsize=(10, 6))

# For each pop value
for i, pop_value in enumerate(sorted(avg_df["pop"].unique())):

    pop_data = avg_df[avg_df["pop"] == pop_value]

    # Plot each timing column
    for col in time_cols:

        plt.plot(
            pop_data["n"],
            pop_data[col]/1_000_000,
            linestyle=line_styles[col],
            marker=markers[i % len(markers)],
            linewidth=2,
            markersize=7,
            label=f"pop={pop_value}, {col}"
        )


x_values = avg_df["n"]

# X-axis labels = nbleaves - 1
x_labels = x_values
plt.xticks(x_values, x_labels, fontsize=14)

# Axes labels and title
#plt.xlabel("n")
plt.ylabel("Average Time")
plt.title("Average Times vs n")

# Log scale for y-axis
plt.yscale("log")

# Styling
plt.legend(ncol=2)
plt.grid(True)

plt.show()