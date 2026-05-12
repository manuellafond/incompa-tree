import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
df = pd.read_csv("../merged.csv")


logscale = True

# Columns to plot
time_columns = ["fasttime", "naivetime", "bhvtime"]

# Group by nbleaves and sum the timing columns
grouped = (
    df.groupby("nbleaves")[time_columns]
    .sum()
    .reset_index()
    .sort_values("nbleaves")
)

print(grouped)

# X positions
x_values = grouped["nbleaves"]

# X-axis labels = nbleaves - 1
x_labels = [str(v - 1) for v in x_values]

# Create the plot
plt.figure(figsize=(10, 6))

if logscale:
    plt.yscale("log")
#plt.xscale("log")

markers = ['o', 's', '*']
cc = 0
for col in time_columns:
    plt.plot(x_values, grouped[col]  / 1_000_000, marker=markers[cc], label=col)
    cc += 1

# Customize x-axis
plt.xticks(x_values, x_labels, fontsize=14)
plt.tick_params(axis='y', labelsize=14)

# Labels and title
#plt.xlabel("Number of leaves", fontsize=18)
#plt.ylabel("Sum of times (seconds)", fontsize=18)
plt.title("Timing comparison by number of leaves" + (" (logscale)" if logscale else ""), fontsize=18)
plt.legend(fontsize=18)

# Optional: grid
plt.grid(True)

# Show the plot
plt.tight_layout()
#plt.show()


plt.savefig("time_rnd" + ("_logscale" if logscale else "") + ".pdf", format="pdf", bbox_inches="tight")









