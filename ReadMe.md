# Security Constrained Unit Commitment (SCUC) — Full Model Documentation

This document describes the mathematical optimization model implemented in the provided C++ code (“SCUC”).

> **Scope note**
> - The model includes thermal unit commitment, piecewise-linear production costs, startup categories, reserves (with optional shortfall), profiled generators, price-sensitive loads, conventional load curtailment, and a DC network with **PTDF** base-case limits and **LODF** N-1 limits (with soft overflow penalties).

---

## 1) Indices, Sets, and Time Convention

All indices are **0-based**, consistent with the code.

### Sets
- **Time steps**:  
  $\mathcal{T}=\{0,1,\dots,T-1\}$

- **Buses**:  
  $\mathcal{B}=\{0,1,\dots,|\mathcal{B}|-1\}$

- **Transmission lines**:  
  $\mathcal{L}=\{0,1,\dots,|\mathcal{L}|-1\}$

- **Thermal generators**:  
  $\mathcal{G}=\{0,1,\dots,|\mathcal{G}|-1\}$

- **Profiled generators**:  
  $\mathcal{P}=\{0,1,\dots,|\mathcal{P}|-1\}$

- **Price-sensitive loads (PSL)**:  
  $\mathcal{D}=\{0,1,\dots,|\mathcal{D}|-1\}$

- **Reserve requirement products** (each is a spinning requirement time series):  
  $\mathcal{R}=\{0,1,\dots,|\mathcal{R}|-1\}$

- **Reserve-eligible thermal generators**:  
  Let $\mathcal{G}^{res}\subseteq \mathcal{G}$ be the subset of thermal gens that can provide reserve.  
  Code builds a mapping:  
  `map_res`: $\mathcal{G}^{res}\to \{0,1,\dots,|\mathcal{G}^{res}|-1\}$  
  and reserve variables are indexed by $i\in\{0,\dots,|\mathcal{G}^{res}|-1\}$.

- **Relevant N-1 contingency pairs**:  
  The code builds a list of monitored/outage pairs:
  $\mathcal{Q}\subseteq \{(k,\ell)\in\mathcal{L}\times\mathcal{L}: k\neq \ell\}$
  including $(k,\ell)$ if the contingency list contains outage line $k$ and $|\text{LODF}_{\ell,k}|>\varepsilon$.  
  Each pair is indexed by $q\in\{0,\dots,|\mathcal{Q}|-1\}$ with:
  - outage line $k(q)$
  - monitored line $\ell(q)$

---

## 2) Parameters (Inputs)

### 2.1 Thermal generator parameters (for each $g\in\mathcal{G}$)
- Min/max power:  
  $P^{min}_g,\; P^{max}_g$

- Ramp limits:  
  $RU_g,\; RD_g$

- Startup/shutdown power limits (used for tightening):  
  $P^{SUlim}_g,\; P^{SDlim}_g$

- Minimum up/down times:  
  $U_g,\; D_g \in \mathbb{Z}_{\ge 0}$

- No-load cost:  
  $C^{NL}_g$

- Initial status (integer):
  - `init_status_g` $> 0$: unit initially ON for $|init\_status_g|$ periods
  - `init_status_g` $< 0$: unit initially OFF for $|init\_status_g|$ periods

- Initial power:  
  $P^{init}_g$

- Must-run flag and optional fixed commitment status:
  - must-run: `must_run_g` $\in\{0,1\}$
  - optional fixed commitment sequence $fix_{g,t}\in\{0,1\}$ for some times $t$

#### Piecewise-linear production segments (for each thermal $g$)
Let $\mathcal{S}_g=\{0,1,\dots,S_g-1\}$ be the segment set.
- Segment length $\ell_{g,s}$ and slope $c_{g,s}$ for $s\in\mathcal{S}_g$

#### Startup categories / stages (for each thermal $g$)
Let $\mathcal{K}_g=\{0,1,\dots,K_g-1\}$ be the startup stage set.
- Stage $k$ has: $\mathrm{Del}_{g,k}$

---

### 2.2 Profiled generator parameters (for each $p\in\mathcal{P}$)
- Time-varying bounds:  
  $P^{min}_{p,t},\; P^{max}_{p,t}$
- Linear generation cost:  
  $C^{prof}_p$

---

### 2.3 Price-sensitive load parameters (for each $d\in\mathcal{D}$)
- Demand cap and revenue:  
  $D^{max}_{d,t},\quad Rev_{d,t}$

---

### 2.4 Reserve requirement parameters (for each $r\in\mathcal{R}$)
- Required amount:  
  $Req_{r,t}$
- Shortfall penalty (enables shortfall only if positive):  
  $Pen^{res}_r$

---

### 2.5 Conventional (inelastic) load and curtailment penalty
- Nodal demand:  
  $Load_{t,b}$
- Curtailment penalty:  
  $Pen^{curt}$

---

### 2.6 Network parameters (DC approximation)
- PTDF matrix:  
  $PTDF_{\ell,b}$
- LODF matrix:  
  $LODF_{\ell,k}$
- Line limits and penalties:
  - Base-case (normal) limit: $Lim^{N}_{\ell}$
  - Contingency (emergency) limit: $Lim^{E}_{\ell}$
  - Overflow penalty: $Pen^{flow}_{\ell}$

---

## 3) Decision Variables

### 3.1 Thermal UC variables (for each $g\in\mathcal{G},\ t\in\mathcal{T}$)
- Commitment: $u_{g,t}\in\{0,1\}$
- Shutdown: $w_{g,t}\in\{0,1\}$
- Power: $p_{g,t}\ge 0$

#### Startup stage binaries
- $v_{g,k,t}\in\{0,1\}$ for $k\in\mathcal{K}_g$

Define the (implicit) switch-on indicator:
$$y_{g,t}\triangleq \sum_{k\in\mathcal{K}_g} v_{g,k,t}$$

#### Segment production
- $p^{seg}_{g,s,t}\ge 0$ for $s\in\mathcal{S}_g$

Above-min production (not explicit variable in code):
$$p^{above}_{g,t}\triangleq \sum_{s\in\mathcal{S}_g} p^{seg}_{g,s,t}$$

---

### 3.2 Reserves
- Generator reserve: $r_{i,t}\ge 0$ for $i\in\{0,\dots,|\mathcal{G}^{res}|-1\}$
- Reserve shortfall (only if $Pen^{res}_r>0$): $sh_{r,t}\ge 0$

---

### 3.3 Profiled generation
- $p^{prof}_{p,t}\ge 0$

---

### 3.4 Price-sensitive load served
- $s^{psl}_{d,t}\ge 0$

---

### 3.5 Conventional load curtailment
- $curt_{b,t}\ge 0$ with bounds:  
  $0 \le curt_{b,t} \le Load_{t,b}$

---

### 3.6 Network overflow variables (soft limits)
- Base overflow: $ov^{base}_{\ell,t}\ge 0$
- Contingency overflow: $ov^{cont}_{q,t}\ge 0$

---

## 4) Objective Function

Minimize total cost:
$$\min\; \Big( C^{thermal} + C^{startup} + C^{prof} - Rev^{psl} + Pen^{res\_short} + Pen^{curt} + Pen^{flow} \Big)$$

### 4.1 Thermal no-load + energy
$$C^{thermal} = \sum_{g}\sum_{t} C^{NL}_g\,u_{g,t} + \sum_{g}\sum_{s}\sum_{t} c_{g,s}\,p^{seg}_{g,s,t}$$

### 4.2 Startup costs
$$C^{startup} = \sum_{g}\sum_{k}\sum_{t} C^{SU}_{g,k}\,v_{g,k,t}$$

### 4.3 Profiled generation cost
$$C^{prof}=\sum_{p}\sum_{t} C^{prof}_p\,p^{prof}_{p,t}$$

### 4.4 PSL revenue (subtracted)
$$Rev^{psl}=\sum_{d}\sum_{t} Rev_{d,t}\,s^{psl}_{d,t}$$

### 4.5 Reserve shortfall penalty (only if $Pen^{res}_r>0$)
$$Pen^{res\_short} = \sum_{r:Pen^{res}_r>0}\sum_{t} Pen^{res}_r\,sh_{r,t}$$

### 4.6 Curtailment penalty
$$Pen^{curt}=\sum_{b}\sum_{t} Pen^{curt}\,curt_{b,t}$$

### 4.7 Line overflow penalties
$$Pen^{flow} = \sum_{\ell}\sum_{t} Pen^{flow}_\ell\,ov^{base}_{\ell,t} + \sum_{q}\sum_{t} Pen^{flow}_{\ell(q)}\,ov^{cont}_{q,t}$$

---

## 5) Constraints

### 5.1 System power balance

For each $t\in\mathcal{T}$:
$$\sum_{g} p_{g,t} + \sum_{p} p^{prof}_{p,t} + \sum_{b} curt_{b,t} - \sum_{d} s^{psl}_{d,t} = \sum_{b} Load_{t,b}$$

---

### 5.2 Reserve requirements

For each reserve product $r\in\mathcal{R}$ and time $t\in\mathcal{T}$:

- If $Pen^{res}_r>0$:
$$\sum_{i} r_{i,t} + sh_{r,t} \ge Req_{r,t}$$

- If $Pen^{res}_r\le 0$:
$$\sum_{i} r_{i,t} \ge Req_{r,t}$$

---

## 6) Thermal Generator Constraints

Fix a generator $g\in\mathcal{G}$.

### 6.1 Initial boundary fixes (remaining min up/down)

Let $h_g=|init\_status_g|$, and $initOn_g = \mathbf{1}[init\_status_g>0]$.

If $initOn_g = 1$, define $remUp_g=\max(0,U_g-h_g)$ and enforce:
$$u_{g,t}=1 \quad \forall t=0,\dots,\min(T-1,remUp_g-1)$$

If $initOn_g = 0$, define $remDn_g=\max(0,D_g-h_g)$ and enforce:
$$u_{g,t}=0 \quad \forall t=0,\dots,\min(T-1,remDn_g-1)$$

---

### 6.2 Initial transition at $t=0$
$$u_{g,0}-initOn_g = y_{g,0}-w_{g,0}$$

#### 6.2a Optional tightening at $t=0$ (enforced in code)
$$w_{g,0} \le initOn_g$$
$$y_{g,0} \le 1-initOn_g$$

---

### 6.3 Power definition
$$p_{g,t}=P^{min}_g\,u_{g,t}+\sum_{s\in\mathcal{S}_g}p^{seg}_{g,s,t}$$

---

### 6.4 Segment bounds
$$0\le p^{seg}_{g,s,t}\le \ell_{g,s}\,u_{g,t}$$

---

### 6.5 Commitment logic for $t\ge 1$
For $t=1,\dots,T-1$:
$$u_{g,t}-u_{g,t-1}=y_{g,t}-w_{g,t}$$

---

### 6.6 Start/shut gating (as in code)
For all $t$:
- Startup implies on:
$$y_{g,t} \le u_{g,t}$$
- For $t\ge 1$, startup only if was previously off:
$$y_{g,t} \le 1-u_{g,t-1}$$
- For $t\ge 1$, shutdown only if was previously on:
$$w_{g,t} \le u_{g,t-1}$$
- For $t\ge 1$, shutdown implies off now:
$$w_{g,t}+u_{g,t}\le 1$$

---

### 6.7 One action per period
$$y_{g,t}+w_{g,t}\le 1 \quad \forall t\in\mathcal{T}$$

---

### 6.8 Minimum up/down time (cumulative)

Minimum up-time:
$$\sum_{\tau=\max(0,t-U_g+1)}^{t} y_{g,\tau} \le u_{g,t} \quad \forall t\in\mathcal{T}$$

Minimum down-time (equivalent to code’s form):
$$\sum_{\tau=\max(0,t-D_g+1)}^{t} w_{g,\tau} \le 1-u_{g,t} \quad \forall t\in\mathcal{T}$$

---

### 6.9 Ramping (implemented on above-min)

Define:
$$p^{above}_{g,t}=\sum_{s\in\mathcal{S}_g} p^{seg}_{g,s,t}.$$
If $g$ is reserve-eligible, define $r_{g,t}=r_{\text{map\_res}(g),t}$; otherwise $r_{g,t}=0$.

Ramp-up ($t\ge 1$):
$$p^{above}_{g,t}+r_{g,t}-p^{above}_{g,t-1}\le RU_g$$

Ramp-down ($t\ge 1$):
$$p^{above}_{g,t-1}-p^{above}_{g,t}\le RD_g$$

Initial ramps ($t=0$):  
Let $initOn_g = \mathbf{1}[init\_status_g>0]$ and
$$p^{init,above}_g= \begin{cases} \max(0, P^{init}_g-P^{min}_g) & \text{if } initOn_g=1 \\ 0 & \text{if } initOn_g=0 \end{cases}$$
Then:
$$p^{above}_{g,0}+r_{g,0}\le p^{init,above}_g+RU_g$$
$$p^{above}_{g,0}\le RD_g - p^{init,above}_g$$

---

### 6.10 Startup/shutdown tightening

Let:
$$Cap^{above}_g=P^{max}_g-P^{min}_g$$
$$A_g=\max(0,P^{max}_g-P^{SUlim}_g),\quad B_g=\max(0,P^{max}_g-P^{SDlim}_g)$$

Startup limit (all $t$):
$$p^{above}_{g,t}+r_{g,t}\le Cap^{above}_g\,u_{g,t}-A_g\,y_{g,t}$$

Shutdown limit (for $t=0,\dots,T-2$):
$$p^{above}_{g,t}\le Cap^{above}_g\,u_{g,t}-B_g\,w_{g,t+1}$$

---

### 6.11 Must-run / fixed commitment
If must-run:
$$u_{g,t}=1 \quad \forall t$$
If fixed commitment $fix_{g,t}$ is provided:
$$u_{g,t}=fix_{g,t} \quad \text{for those } t$$

---

### 6.12 Reserve headroom (reserve-eligible only)
For $g\in\mathcal{G}^{res}$:
$$p_{g,t}+r_{\text{map\_res}(g),t}\le P^{max}_g\,u_{g,t}$$

---

## 7) Startup Category Logic

This section describes the exact logic enforced by the code for choosing startup categories.

Assume stages are indexed so that:
$$\mathrm{Del}_{g,0}\le \mathrm{Del}_{g,1}\le \dots \le \mathrm{Del}_{g,K_g-1}$$

Let $initOn_g = \mathbf{1}[init\_status_g>0]$ and if $initOn_g = 0$ define initial offline duration:
$$initOffDur_g = -init\_status_g \quad (\text{else } initOffDur_g=0).$$

The code adds constraints only for categories $k=0,\dots,K_g-2$ (the last category is not restricted by this routine).

For each time $t\in\mathcal{T}$ and each $k\in\{0,\dots,K_g-2\}$, define:
- An initial-status allowance term $L^{init}_{g,k,t}\in\{0,1\}$:
  - If the unit is initially OFF, define $offdur = initOffDur_g + t$.
  - Then:
    $$L^{init}_{g,k,t} = \begin{cases} 1 & \text{if } offdur \in [\mathrm{Del}_{g,k},\, \mathrm{Del}_{g,k+1}-1] \\ 0 & \text{otherwise} \end{cases}$$
  - If initially ON, then $L^{init}_{g,k,t}=0$.

- A shutdown window:
  $$lb = \max(0,\; t-\mathrm{Del}_{g,k+1}+1), \qquad ub = t-\mathrm{Del}_{g,k}.$$

Then the code enforces:
$$v_{g,k,t} - \sum_{i=lb}^{ub} w_{g,i} \le L^{init}_{g,k,t}, \quad \text{whenever } ub\ge 0 \text{ and } lb\le ub.$$

If the window is empty (i.e., $ub<0$ or $lb>ub$), the constraint reduces to:
$$v_{g,k,t} \le L^{init}_{g,k,t}.$$

**Interpretation (code-consistent):**
- Selecting startup category $k$ at time $t$ is permitted if either:
  - the **initial offline duration** implies category $k$ is feasible at time $t$ (via $L^{init}=1$), or
  - there has been at least one **shutdown event** $w_{g,i}=1$ in the window $i\in[lb,ub]$, i.e., a shutdown occurred at a time consistent with being offline for a duration in $[\mathrm{Del}_{g,k},\,\mathrm{Del}_{g,k+1}-1]$.

The **coldest** category $K_g-1$ is not restricted by this routine and is therefore always feasible (subject to the other startup constraints like $y_{g,t}\le u_{g,t}$ and $y_{g,t}\le 1-u_{g,t-1}$).

---

## 8) Profiled Generator Constraints
For each $p\in\mathcal{P}$ and $t\in\mathcal{T}$:
$$P^{min}_{p,t}\le p^{prof}_{p,t}\le P^{max}_{p,t}$$

---

## 9) Price-Sensitive Load Constraints
For each $d\in\mathcal{D}$ and $t\in\mathcal{T}$:
$$0\le s^{psl}_{d,t}\le D^{max}_{d,t}$$

---

## 10) Curtailment Bounds
For each $b\in\mathcal{B}$ and $t\in\mathcal{T}$:
$$0\le curt_{b,t}\le Load_{t,b}$$

---

## 11) Network Model (PTDF + LODF, soft)

The code does not explicitly create net-injection or flow variables; it builds line constraints directly by expanding injections into generator/load variables using PTDF rows.

### 11.1 Net injections (conceptual)
For bus $b$ and time $t$:
$$inj_{b,t}= \sum_{g\in\mathcal{G}(b)}p_{g,t} + \sum_{p\in\mathcal{P}(b)}p^{prof}_{p,t} - \sum_{d\in\mathcal{D}(b)}s^{psl}_{d,t} - Load_{t,b} + curt_{b,t}$$

### 11.2 Base flow (conceptual)
$$flow^{base}_{\ell,t}=\sum_{b} PTDF_{\ell,b}\,inj_{b,t}$$

### 11.3 Base soft limits
For each line $\ell$ and time $t$:
$$flow^{base}_{\ell,t}\le Lim^{N}_{\ell}+ov^{base}_{\ell,t}$$
$$-flow^{base}_{\ell,t}\le Lim^{N}_{\ell}+ov^{base}_{\ell,t}$$

### 11.4 Contingency flow for pair $q=(k(q),\ell(q))$
Let $k=k(q)$ and $\ell=\ell(q)$. Then:
$$flow^{cont}_{q,t} = \sum_{b}\Big(PTDF_{\ell,b}+LODF_{\ell,k}\,PTDF_{k,b}\Big)\,inj_{b,t}$$

### 11.5 Contingency soft limits
For each pair $q$ and time $t$:
$$flow^{cont}_{q,t}\le Lim^{E}_{\ell(q)}+ov^{cont}_{q,t}$$
$$-flow^{cont}_{q,t}\le Lim^{E}_{\ell(q)}+ov^{cont}_{q,t}$$

---

## 12) Summary

This SCUC model chooses:
- thermal unit on/off schedules and dispatch,
- startup categories and costs,
- reserve provision (and optional shortfall),
- profiled generation within availability bounds,
- price-sensitive load served if profitable,
- load curtailment only if necessary (penalized),
- while enforcing base-case and N-1 network limits using PTDF/LODF with soft overflow penalties.
