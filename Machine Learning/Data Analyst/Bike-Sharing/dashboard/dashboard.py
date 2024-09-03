import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import streamlit as st
# from babel.numbers import format_currency
sns.set_theme(style='dark')

def create_daily_rent_df(df):
    daily_rent_df = df.resample(rule='D', on='date').agg({
        'total_rental_bikes' : 'sum'
    })
    daily_rent_df = daily_rent_df.reset_index()
    return daily_rent_df

def create_monthly_rent_df(df):
    monthly_rent_df = df.resample(rule='M', on='date').agg({
        'total_rental_bikes' : 'sum'
    })
    monthly_rent_df = monthly_rent_df.reset_index()
    return monthly_rent_df

def create_rent_by_season_df(df):
    rent_by_season_df = df.groupby(by='season').total_rental_bikes.mean().sort_values(ascending = False).reset_index()
    return rent_by_season_df

def create_working_day_df(df):
    working_day_df = df.groupby(by='workingday').total_rental_bikes.sum().reset_index()
    return working_day_df

bike_df = pd.read_csv("bike_df.csv")
datetime_columns = ['date']
bike_df.sort_values(by='date', inplace=True)
bike_df.reset_index(inplace=True)

for column in datetime_columns:
    bike_df[column] = pd.to_datetime(bike_df['date'])

min_date = bike_df['date'].min()
max_date = bike_df['date'].max()

# bike_df['date'] = pd.to_datetime(bike_df['date'])
min_date = bike_df['date'].min()
max_date = bike_df['date'].max()


with st.sidebar:
    # Menambahkan logo perusahaan
    st.image("https://i.pinimg.com/564x/70/cb/f5/70cbf5630b2ea046cd18d0cb29f0ae68.jpg")
    
    # Mengambil start_date & end_date dari date_input
    start_date, end_date = st.date_input(
        label='Rentang Waktu',
        min_value=min_date,
        max_value=max_date,
        value=[min_date, max_date]
    )

    time_df = bike_df[(bike_df['date'] >= str(start_date)) & 
                (bike_df['date'] <= str(end_date))]
    daily_rent_df = create_daily_rent_df(time_df)
    monthly_rent_df = create_monthly_rent_df(time_df)
    rent_by_season_df = create_rent_by_season_df(time_df)
    working_day_df = create_working_day_df(time_df)

st.set_option('deprecation.showPyplotGlobalUse', False)

# Create a streamlit figure

st.subheader('Total Rents')
col1 = st.columns(1)[0]
with col1:
    total_rent = monthly_rent_df.total_rental_bikes.sum()
    st.metric("Total rents", value=total_rent)

fig, ax = plt.subplots(figsize=(10, 5))
ax.plot(daily_rent_df["date"], daily_rent_df["total_rental_bikes"], marker='o', linewidth=2, color="#72BCD4")
ax.set_title("Daily Bikes Rental Trend", loc="center", fontsize=20)
ax.tick_params(axis='x', labelrotation=45, labelsize=10)
ax.tick_params(axis='y', labelsize=10)
st.pyplot(fig)

fig, ax = plt.subplots(figsize=(10, 5))
ax.plot(monthly_rent_df["date"], monthly_rent_df["total_rental_bikes"], marker='o', linewidth=2, color="#72BCD4")
ax.set_title("Monthly Bikes Rental Trend", loc="center", fontsize=20)
ax.tick_params(axis='x', labelrotation=45, labelsize=10)
ax.tick_params(axis='y', labelsize=10)
st.pyplot(fig)

st.header('Pengaruh perubahan musim terhadap perentalan sepeda')
fig, ax = plt.subplots(figsize=(10, 5))
sns.barplot(
    data = rent_by_season_df, 
    x="season",
    y="total_rental_bikes",
    # palette=["#72BCD4" if season == rent_by_season_df['season'].iloc[2] else "#D3D3D3" for season in rent_by_season_df['season']]
)

ax.set_title("Number of Customer by Gender", loc="center", fontsize=15)
ax.set_xlabel(None)
ax.set_ylabel(None)
ax.tick_params(axis='x', labelsize=12)
st.pyplot()

st.header('Pengaruh perbedaan hari kerja terhadap perentalan sepeda')
labels = working_day_df['workingday']
sizes = working_day_df['total_rental_bikes']
colors = ['#D3D3D3', '#72BCD4']

# Membuat pie chart
fig, ax = plt.subplots(figsize=(8, 6))
ax.pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%', startangle=140)

ax.set_title('Percentage of Total Rental Bikes by Working Day')
ax.axis('equal')
st.pyplot(fig)
